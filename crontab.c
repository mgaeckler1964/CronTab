/*
		Project:		Crontab
		Module:			crontab,c
		Description:	My implemetation of crontab for windows.
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2024 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Austria, Linz ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

#include <stdio.h>
#include <time.h>

#include <windows.h>

#include "cronconst.h"

#define RUN_AS_SERVICE

#ifdef _MSC_VER
typedef enum { false, true } bool;
#endif

typedef struct
{
	char	cronJobTitle[BUFFER_SIZE];
	char	commandLine[BUFFER_SIZE];
	bool	multipleInst;
	time_t	sleeper;
	int		nextDay;
	int		nextMonth;
	int		nextYear;
	int		nextHour;
	int		nextMinute;
	int		nextSecond;
	long	interval;
	HANDLE	pid;
} CRON_JOB;

static size_t	numCronJobs		= 0;
static size_t	jobsRunning		= 0;
static CRON_JOB	*jobList		= NULL;

static SERVICE_STATUS			MyServiceStatus;
static SERVICE_STATUS_HANDLE	MyServiceStatusHandle;

static volatile int				servicePaused = 0;
static volatile int				serviceStopped = 0;

/*
	Write a message to the event log

	can be read with the event viewer program
*/
static void EventLog( int type, const char *format, ... )
{
	va_list	args;

	HANDLE	eventHndl;
	char	message[1024];
	char	*msg = message;

	va_start( args, format );
	vsprintf( message, format, args );
	va_end( args );

	eventHndl = RegisterEventSource( NULL, SERVICE_NAME );

	ReportEvent( eventHndl, (WORD)type, 0, 0, 0, 1, 0, (LPCTSTR*)&msg, NULL );
	DeregisterEventSource( eventHndl );
}

/*
	read the job list written by the cron tab configuration utility
*/
static void readJobList( void )
{
	DWORD	dummy;
	long	openResult;
	HKEY	softKey;
	HKEY	cresdKey;
	HKEY	crontabKey;
	HKEY	jobKey;

	DWORD	index;
	char	nextStart[BUFFER_SIZE];
	char	interval[BUFFER_SIZE];
	char	intervalType[BUFFER_SIZE];
	char	multiInst[BUFFER_SIZE];

	DWORD	numSubKeys;
	DWORD	size;

	openResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, SOFTWARE_KEY, 0,
								KEY_CREATE_SUB_KEY, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegCreateKeyEx(	softKey, COMPANY, 0, "",
										REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS,
										NULL,
										&cresdKey,
										&dummy );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = RegCreateKeyEx(	cresdKey, SERVICE_NAME, 0, "",
											REG_OPTION_NON_VOLATILE,
											KEY_ALL_ACCESS,
											NULL,
											&crontabKey,
											&dummy );
			if( openResult == ERROR_SUCCESS )
			{
				openResult = RegQueryInfoKey( crontabKey, NULL, NULL, NULL,
												&numSubKeys,
												NULL, NULL, NULL,
												NULL, NULL, NULL,
												NULL );
				if( openResult == ERROR_SUCCESS )
				{
					if( jobList )
						free( jobList );
					jobList = (CRON_JOB*)malloc( numSubKeys * sizeof( *jobList ) );
					if( jobList )
					{
						numCronJobs = numSubKeys;
						for( index = 0; index < numSubKeys; index++ )
						{
							size = BUFFER_SIZE;
							openResult = RegEnumKeyEx( crontabKey, index, jobList[index].cronJobTitle, &size, NULL, NULL, NULL, NULL );
							if( openResult == ERROR_SUCCESS )
							{
								openResult = RegOpenKeyEx( crontabKey, jobList[index].cronJobTitle, 0, KEY_READ, &jobKey );
								if( openResult == ERROR_SUCCESS )
								{
									size = BUFFER_SIZE;
									RegQueryValueEx( jobKey, COMMAND_LINE, NULL, NULL, (unsigned char *)jobList[index].commandLine, &size );
									size = BUFFER_SIZE;
									RegQueryValueEx( jobKey, INTERVAL, NULL, NULL, (unsigned char *)interval, &size );
									size = BUFFER_SIZE;
									RegQueryValueEx( jobKey, INTERVAL_TYPE, NULL, NULL, (unsigned char *)intervalType, &size );
									size = BUFFER_SIZE;
									RegQueryValueEx( jobKey, NEXT_START, NULL, NULL, (unsigned char *)nextStart, &size );
									size = BUFFER_SIZE;
									RegQueryValueEx( jobKey, MULTIPLE_INST, NULL, NULL, (unsigned char *)multiInst, &size );


									jobList[index].nextDay		=
									jobList[index].nextMonth	=
									jobList[index].nextYear		=
									jobList[index].nextHour		=
									jobList[index].nextMinute	=
									jobList[index].nextSecond	= 0;

									jobList[index].sleeper		= 0;

									sscanf( nextStart, "%d.%d.%d %d:%d:%d",
												&(jobList[index].nextDay),
												&(jobList[index].nextMonth),
												&(jobList[index].nextYear),
												&(jobList[index].nextHour),
												&(jobList[index].nextMinute),
												&(jobList[index].nextSecond) );
									jobList[index].interval = atoi( interval );
									if( !strcmp( intervalType, "Minutes" ) )
										jobList[index].interval *= 60;
									else if( !strcmp( intervalType, "Hours" ) )
										jobList[index].interval *= 60 * 60;
									else if( !strcmp( intervalType, "Days" ) )
										jobList[index].interval *= 60 * 60 * 24;

									jobList[index].multipleInst = multiInst[0] == '1' ? true : false;

									jobList[index].pid = 0;

									EventLog( EVENTLOG_INFORMATION_TYPE, "Found job %s", jobList[index].cronJobTitle );
									EventLog( EVENTLOG_INFORMATION_TYPE, "Found command %s", jobList[index].commandLine );
									EventLog( EVENTLOG_INFORMATION_TYPE, "Found next %d.%d.%d %d:%d:%d",
												jobList[index].nextDay,
												jobList[index].nextMonth,
												jobList[index].nextYear,
												jobList[index].nextHour,
												jobList[index].nextMinute,
												jobList[index].nextSecond );

									RegCloseKey( jobKey );
								}
								else
									jobList[index].commandLine[0] = 0;

							}
						}
					}
				}

				RegCloseKey( crontabKey );
			}

			RegCloseKey( cresdKey );
		}
		RegCloseKey( softKey );
	}
}

/*
	find a job to start
*/
static int findWaitingJob( void )
{
	int			jobToStart;
	time_t		curTime		= time( NULL );
	struct tm	*tmStruct	= NULL;

	for( jobToStart = numCronJobs-1; jobToStart >= 0; jobToStart-- )
	{
		if( jobList[jobToStart].commandLine[0] && !jobList[jobToStart].pid )
		{
			if( jobList[jobToStart].sleeper )
			{
				if( jobList[jobToStart].sleeper < curTime )
					break;
			}
			else
			{
				if( !tmStruct )
				{
					tmStruct	= localtime( &curTime );

					tmStruct->tm_year += 1900;
					tmStruct->tm_mon++;
				}

				if( jobList[jobToStart].nextYear < tmStruct->tm_year )
					break;
				if( jobList[jobToStart].nextYear > tmStruct->tm_year )
					continue;

				if( jobList[jobToStart].nextMonth < tmStruct->tm_mon )
					break;
				if( jobList[jobToStart].nextMonth > tmStruct->tm_mon )
					continue;

				if( jobList[jobToStart].nextDay < tmStruct->tm_mday )
					break;
				if( jobList[jobToStart].nextDay > tmStruct->tm_mday )
					continue;

				if( jobList[jobToStart].nextHour < tmStruct->tm_hour )
					break;
				if( jobList[jobToStart].nextHour > tmStruct->tm_hour )
					continue;

				if( jobList[jobToStart].nextMinute < tmStruct->tm_min )
					break;
				if( jobList[jobToStart].nextMinute > tmStruct->tm_min )
					continue;

				if( jobList[jobToStart].nextSecond < tmStruct->tm_sec )
					break;
				if( jobList[jobToStart].nextSecond > tmStruct->tm_sec )
					continue;
			}
		}
	}
	return jobToStart;
}

/*
	store the next start time of a job in the registration database
*/
static void saveNextStart(	const char *title,
							const char *nextStart )
{
	DWORD			dummy;
	long			openResult;
	HKEY			softKey;
	HKEY			cresdKey;
	HKEY			crontabKey;
	HKEY			jobKey;

	openResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE, SOFTWARE_KEY, 0,
								KEY_CREATE_SUB_KEY, &softKey );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = RegCreateKeyEx(	softKey, COMPANY, 0, "",
										REG_OPTION_NON_VOLATILE,
										KEY_ALL_ACCESS,
										NULL,
										&cresdKey,
										&dummy );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = RegCreateKeyEx(	cresdKey, SERVICE_NAME, 0, "",
											REG_OPTION_NON_VOLATILE,
											KEY_ALL_ACCESS,
											NULL,
											&crontabKey,
											&dummy );
			if( openResult == ERROR_SUCCESS )
			{
				openResult = RegOpenKeyEx(	crontabKey, title, 0,
											KEY_SET_VALUE,
											&jobKey );
				if( openResult == ERROR_SUCCESS )
				{
					RegSetValueEx( jobKey, NEXT_START, 0, REG_SZ, (const unsigned char *)nextStart, strlen( nextStart ) + 1 );
					RegCloseKey( jobKey );
				}

				RegCloseKey( crontabKey );
			}

			RegCloseKey( cresdKey );
		}
		RegCloseKey( softKey );
	}
}

/*
	calc the next start of a given job
*/
static void calcNextStart( size_t jobToStart )
{
	time_t		curTime;
	struct tm	*tmStruct;
	char		nextStart[BUFFER_SIZE];

	curTime = time( NULL );
	curTime += jobList[jobToStart].interval;

	jobList[jobToStart].sleeper = curTime;

	tmStruct = localtime( &curTime );
	tmStruct->tm_year += 1900;
	tmStruct->tm_mon++;

	jobList[jobToStart].nextYear = tmStruct->tm_year;
	jobList[jobToStart].nextMonth = tmStruct->tm_mon;
	jobList[jobToStart].nextDay = tmStruct->tm_mday;
	jobList[jobToStart].nextHour = tmStruct->tm_hour;
	jobList[jobToStart].nextMinute = tmStruct->tm_min;
	jobList[jobToStart].nextSecond = tmStruct->tm_sec;
	sprintf( nextStart, "%d.%d.%d %d:%d:%d",
			jobList[jobToStart].nextDay,
			jobList[jobToStart].nextMonth,
			jobList[jobToStart].nextYear,
			jobList[jobToStart].nextHour,
			jobList[jobToStart].nextMinute,
			jobList[jobToStart].nextSecond );
	saveNextStart(jobList[jobToStart].cronJobTitle, nextStart );
}

/*
	start a job
*/
static void runJob( int index )
{
	STARTUPINFO				startInfo;
	PROCESS_INFORMATION		procInfo;

	memset( &startInfo, 0, sizeof( startInfo ) );
	startInfo.cb = sizeof( startInfo );

//	EventLog( EVENTLOG_INFORMATION_TYPE, "Start command %s", jobList[index].commandLine );

	if( CreateProcess( NULL, jobList[index].commandLine,
						NULL, NULL, FALSE, CREATE_SEPARATE_WOW_VDM,
						NULL, NULL, &startInfo, &procInfo ) )
	{
		if( !jobList[index].multipleInst )
		{
			jobList[index].pid = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_TERMINATE, FALSE, procInfo.dwProcessId );
			jobsRunning++;
		}
		else
		{
			calcNextStart( index );
		}
	}
	else
	{
		EventLog( EVENTLOG_ERROR_TYPE, "Cannot start command %s", jobList[index].commandLine );
		jobList[index].commandLine[0] = 0;
	}
}

/*
	find out whether a previously started job has terminated
*/
static void calcNextStart4All( void )
{
	int			jobToStart;
	DWORD		exitCode;

	for( jobToStart = numCronJobs-1; jobToStart >= 0; jobToStart-- )
	{
		if( jobList[jobToStart].pid )
		{
			GetExitCodeProcess( jobList[jobToStart].pid, &exitCode );
			if( exitCode != STILL_ACTIVE )
			{
//				EventLog( EVENTLOG_INFORMATION_TYPE, "%s terminated", jobList[jobToStart].commandLine );
				calcNextStart( jobToStart );

				CloseHandle( jobList[jobToStart].pid );
				jobList[jobToStart].pid = 0;
				jobsRunning--;
			}
		}
	}
}

/*
	find minimum time to sleep

	NOTE: we do no sleep longer than 15 seconds
	otherwise we cannot handle the service control
	function
*/
static unsigned long findSleepTime( void )
{
	if( jobsRunning )
		return 1000;
	else
	{
		unsigned long	minSleepTime = 15;	// wait max. 15 seconds;
		unsigned long	sleepTime;
		time_t			curTime		= time( NULL );
		int				i;

		for( i = numCronJobs-1; i >= 0; i-- )
		{
			if( jobList[i].sleeper > curTime )
			{
				sleepTime = (unsigned long)(jobList[i].sleeper - curTime);
				if( sleepTime < minSleepTime )
					minSleepTime = sleepTime;
			}
			else if( jobList[i].sleeper )
				minSleepTime = 0;
		}

		return minSleepTime * 1000;
	}
}

// this is the main function that will start the cron jobs
static void StartServiceThread( void )
{
	int		jobToStart;

	readJobList();

	while( !serviceStopped )
	{
		while( servicePaused )
			Sleep( 1000 );

		if( jobsRunning < numCronJobs )
		{
			jobToStart = findWaitingJob();
			if( jobToStart >= 0 )
				runJob( jobToStart );
		}

		if( jobsRunning )
			calcNextStart4All();

		Sleep( findSleepTime() );
	}
}

#ifdef RUN_AS_SERVICE
static void WINAPI MyServiceCtrlHandler ( DWORD Opcode )
{
	switch( Opcode )
	{
	case SERVICE_CONTROL_PAUSE:
		servicePaused = 1;
		MyServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;

	case SERVICE_CONTROL_CONTINUE:
		servicePaused = 0;
		MyServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;

	case SERVICE_CONTROL_STOP:
		serviceStopped = 1;
		MyServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		MyServiceStatus.dwWin32ExitCode = 0;
		MyServiceStatus.dwCheckPoint    = 0;
		MyServiceStatus.dwWaitHint      = 0;
		break;

	case SERVICE_CONTROL_INTERROGATE:
		break;
	 }

	SetServiceStatus (MyServiceStatusHandle,  &MyServiceStatus);
	return;
}

static void WINAPI ServiceMain( DWORD x, LPSTR *y )
{
	// for a service we only register the control handler no log window
	MyServiceStatus.dwServiceType        = SERVICE_WIN32_OWN_PROCESS;
	MyServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
	MyServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	MyServiceStatus.dwWin32ExitCode      = 0;
	MyServiceStatus.dwServiceSpecificExitCode = 0;
	MyServiceStatus.dwCheckPoint         = 0;
	MyServiceStatus.dwWaitHint           = 0;

	MyServiceStatusHandle = RegisterServiceCtrlHandler(
							SERVICE_NAME,
							MyServiceCtrlHandler);

	MyServiceStatus.dwCurrentState       = SERVICE_RUNNING;
	MyServiceStatus.dwCheckPoint         = 0;
	MyServiceStatus.dwWaitHint           = 0;

	SetServiceStatus( MyServiceStatusHandle, &MyServiceStatus );

	StartServiceThread();

	MyServiceStatus.dwCurrentState       = SERVICE_STOPPED;
	MyServiceStatus.dwCheckPoint         = 0;
	MyServiceStatus.dwWaitHint           = 0;

	SetServiceStatus (MyServiceStatusHandle, &MyServiceStatus);
}
#endif

int main( int argc, char **argv )
{
#ifdef RUN_AS_SERVICE
	SERVICE_TABLE_ENTRY   DispatchTable[] =
	{
		{ SERVICE_NAME,	ServiceMain		},
		{ NULL,			NULL			}
	};

	StartServiceCtrlDispatcher( DispatchTable );
#else
	StartServiceThread();
#endif
	return 0;
}

