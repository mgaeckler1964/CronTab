/*
		Project:		Crontab
		Module:			CronConf.cpp
		Description:	The configuration utility for my cron tab
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2025 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
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

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/fmtNumber.h>
#include <winlib/WINAPP.H>

#include "CronConf.gui.h"
#include "cronconst.h"

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

using namespace winlib;
using namespace winlibGUI;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

#define JOB_OK			0
#define JOB_BAD_DELETE	1
#define JOB_BAD_INSERT	2
#define JOB_EXISTS		3
#define JOB_BAD_READ	4

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class CreateEditJobForm : public CreateEditJobForm_form
{
	STRING	m_jobTitle;

	virtual ProcessStatus handleCreate();
	virtual ProcessStatus handleOk();

public:
	CreateEditJobForm() : CreateEditJobForm_form( NULL ) {}

	void setJobTitle( const STRING &newJobTitle )
	{
		m_jobTitle = newJobTitle;
	}
	const STRING &getJobTitle() const
	{
		return m_jobTitle;
	}
};

class CronConfMainWindow : public cronConfMainForm_form
{
	void checkService();

	void AddCronJob();
	void EditCronJob();
	void DeleteJob();

	void InstallService();
	void RemoveService();
	void StopService();
	void StartService();

	virtual ProcessStatus handleCreate();
	virtual ProcessStatus handleButtonClick( int control );

public:
	CronConfMainWindow() : cronConfMainForm_form( NULL ) {}
};

class CronConfApplication : public GuiApplication
{
	virtual CallbackWindow  *createMainWindow( const char * /*cmdLine*/, int /*nCmdShow*/ )
	{
		doEnableLogEx(gakLogging::llInfo);
		CronConfMainWindow	*mainWindow = new CronConfMainWindow;
		if( mainWindow->create( NULL ) == scERROR )
		{
			MessageBox( NULL, "Could not create window", "Error", MB_ICONERROR );
			delete mainWindow;
			mainWindow = NULL;
		}

		return mainWindow;
	}
	virtual void deleteMainWindow( BasicWindow  *mainWindow )
	{
		delete mainWindow;
	}

	public:
	CronConfApplication() : GuiApplication( IDI_SDIAPPLICATION ) {}
};

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

static CronConfApplication	cronConfApplication;

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

static int readJob(	const char *jobTitle,
					STRING	*command,
					STRING	*nextStart,
					STRING	*interval,
					STRING	*intervalType,
					bool	*multipleInst )
{
	DWORD			dummy;
	long			openResult;
	HKEY			softKey;
	HKEY			cresdKey;
	HKEY			crontabKey;
	HKEY			jobKey;
	unsigned char	multiple[16];
	
	DWORD			size;

	int				result = JOB_BAD_READ;

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
				openResult = RegCreateKeyEx(	crontabKey, jobTitle, 0, "",
												REG_OPTION_NON_VOLATILE,
												KEY_ALL_ACCESS,
												NULL,
												&jobKey,
												&dummy );
				if( openResult == ERROR_SUCCESS )
				{
					size = 0;
					openResult = RegQueryValueEx( jobKey, COMMAND_LINE, NULL, NULL, (LPBYTE)"", &size );
					if( openResult == ERROR_MORE_DATA )
					{
						command->setMinSize(size+1);
					}
					RegQueryValueEx( jobKey, COMMAND_LINE, NULL, NULL, (LPBYTE)command->c_str(), &size );
					command->setActSize( size-1 );

					size = 0;
					RegQueryValueEx( jobKey, INTERVAL, NULL, NULL, (LPBYTE)"", &size );
					if( openResult == ERROR_MORE_DATA )
					{
						interval->setMinSize(size+1);
					}
					RegQueryValueEx( jobKey, INTERVAL, NULL, NULL, (LPBYTE)interval->c_str(), &size );
					interval->setActSize(size-1);

					size = 0;
					RegQueryValueEx( jobKey, INTERVAL_TYPE, NULL, NULL, (LPBYTE)"", &size );
					if( openResult == ERROR_MORE_DATA )
					{
						intervalType->setMinSize(size+1);
					}
					RegQueryValueEx( jobKey, INTERVAL_TYPE, NULL, NULL, (LPBYTE)intervalType->c_str(), &size );
					intervalType->setActSize(size-1);

					size = 0;
					RegQueryValueEx( jobKey, NEXT_START, NULL, NULL, (LPBYTE)"", &size );
					if( openResult == ERROR_MORE_DATA )
					{
						nextStart->setMinSize(size+1);
					}
					RegQueryValueEx( jobKey, NEXT_START, NULL, NULL, (LPBYTE)nextStart->c_str(), &size );
					nextStart->setActSize(size-1);

					size = sizeof( multiple );
					*multiple = 0;
					RegQueryValueEx( jobKey, MULTIPLE_INST, NULL, NULL, multiple, &size );
					if( *multiple == '1' )
						*multipleInst = true;
					else
						*multipleInst = false;

					result = JOB_OK;
					RegCloseKey( jobKey );
				}
				else
					result = JOB_BAD_INSERT;

				RegCloseKey( crontabKey );
			}

			RegCloseKey( cresdKey );
		}
		RegCloseKey( softKey );
	}

	return result;
}

static int writeJob(	const char *oldJob, const char *newTitle,
						const char *command,
						const char *nextStart,
						const char *interval,
						const char *intervalType,
						bool		multipleInstances )
{
	DWORD			dummy;
	long			openResult;
	HKEY			softKey;
	HKEY			cresdKey;
	HKEY			crontabKey;
	HKEY			jobKey;
	char			multipleInst[2];

	int				result = JOB_BAD_INSERT;

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
				jobKey = NULL;
				if( strcmp( oldJob, newTitle ) )
				{
					openResult = RegOpenKeyEx( crontabKey, newTitle, 0, KEY_READ, &jobKey );
					if( openResult == ERROR_SUCCESS )
					{
						result = JOB_EXISTS;
						RegCloseKey( jobKey );
					}
				}
				if( !jobKey )
				{
					openResult = ERROR_SUCCESS;
					if( *oldJob )
					{
						openResult = RegDeleteKey( crontabKey, oldJob );
						if( openResult != ERROR_SUCCESS )
							result = JOB_BAD_DELETE;
					}
					if( openResult == ERROR_SUCCESS )
					{
						openResult = RegCreateKeyEx(	crontabKey, newTitle, 0, "",
														REG_OPTION_NON_VOLATILE,
														KEY_ALL_ACCESS,
														NULL,
														&jobKey,
														&dummy );
						if( openResult == ERROR_SUCCESS )
						{
							RegSetValueExA( jobKey, COMMAND_LINE, 0, REG_SZ, (const unsigned char *)command, DWORD(strlen( command ) + 1) );
							RegSetValueEx( jobKey, INTERVAL, 0, REG_SZ, (const unsigned char *)interval, DWORD(strlen( interval ) + 1) );
							RegSetValueEx( jobKey, INTERVAL_TYPE, 0, REG_SZ, (const unsigned char *)intervalType, DWORD(strlen( intervalType ) + 1) );
							RegSetValueEx( jobKey, NEXT_START, 0, REG_SZ, (const unsigned char *)nextStart, DWORD(strlen( nextStart ) + 1) );

							multipleInst[0] = multipleInstances ? '1' : '0';
							multipleInst[1] = 0;
							RegSetValueEx( jobKey, MULTIPLE_INST, 0, REG_SZ, (const unsigned char *)multipleInst, DWORD(strlen( multipleInst ) + 1) );

							result = JOB_OK;
							RegCloseKey( jobKey );
						}
						else
							result = JOB_BAD_INSERT;
					}
				}

				RegCloseKey( crontabKey );
			}

			RegCloseKey( cresdKey );
		}
		RegCloseKey( softKey );
	}

	return result;
}

static void deleteCronJob( const char *jobTitle )
{
	DWORD			dummy;
	long			openResult;
	HKEY			softKey;
	HKEY			cresdKey;
	HKEY			crontabKey;

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
				RegDeleteKey( crontabKey, jobTitle );
				RegCloseKey( crontabKey );
			}

			RegCloseKey( cresdKey );
		}
		RegCloseKey( softKey );
	}
}

static char *getServiceStatus( DWORD status )
{
	switch( status )
	{
		case SERVICE_STOPPED:			return "not running.";
		case SERVICE_START_PENDING:		return "starting.";
		case SERVICE_STOP_PENDING:		return "stopping.";
		case SERVICE_RUNNING:			return "running.";
		case SERVICE_CONTINUE_PENDING:	return "continue is pending.";
		case SERVICE_PAUSE_PENDING:		return "pause is pending.";
		case SERVICE_PAUSED:			return "paused.";
		default:						return "unknown status";
	}
}

static void readCronJobs( ListBox *cronJobs )
{
	DWORD			dummy;
	long			openResult;
	HKEY			softKey;
	HKEY			cresdKey;
	HKEY			crontabKey;

	DWORD			index;

	cronJobs->clearEntries();

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
				STRING cronJobTitle;
				index = 0;

				DWORD maxTitleSize;
				RegQueryInfoKey(crontabKey, NULL, NULL, NULL, NULL, &maxTitleSize, NULL, NULL, NULL, NULL, NULL, NULL );
				++maxTitleSize;
				while( 1 )
				{
					DWORD titleSize = maxTitleSize;
					cronJobTitle.setMinSize(maxTitleSize+1);
					openResult = RegEnumKeyEx( crontabKey, index++, (LPSTR)cronJobTitle.c_str(), &titleSize, NULL, NULL, NULL, NULL );
					if( openResult == ERROR_SUCCESS )
					{
						cronJobTitle.setActSize(titleSize);
						cronJobs->addEntry( cronJobTitle );
					}
					else
						break;
				}

				RegCloseKey( crontabKey );
			}

			RegCloseKey( cresdKey );
		}
		RegCloseKey( softKey );
	}
}

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void CronConfMainWindow::AddCronJob()
{
	CreateEditJobForm	dialog;

	if( dialog.create( this ) == scSUCCESS )
	{
		if( dialog.getModalResult() == IDOK )
		{
			readCronJobs( cronJobs );
		}
	}

}


void CronConfMainWindow::EditCronJob()
{
	STRING	jobTitle = cronJobs->getSelectedItems();

	if( !jobTitle.isEmpty() )
	{
		CreateEditJobForm	dialog;
		dialog.setJobTitle( jobTitle );
		if( dialog.create( this ) == scSUCCESS )
		{
			if( dialog.getModalResult() == IDOK )
			{
				readCronJobs( cronJobs );
				cronJobs->selectEntry( dialog.getJobTitle() );
			}
		}
	}
}

void CronConfMainWindow::DeleteJob()
{
	STRING	jobTitle = cronJobs->getSelectedItems();

	if( !jobTitle.isEmpty() )
	{
		if( messageBox( "Do you want to delete the selected job?", "Delete Confirm", MB_YESNO|MB_ICONQUESTION ) == IDYES )
		{
			deleteCronJob( jobTitle );
			readCronJobs( cronJobs );
		}
	}
}


void CronConfMainWindow::checkService()
{
	SC_HANDLE	serviceManager;
	SC_HANDLE	jobService;

	serviceManager = OpenSCManager( NULL, NULL, SC_MANAGER_CONNECT );
	if( serviceManager )
	{
		SERVICE_STATUS	status;

		jobService = OpenService( serviceManager, SERVICE_NAME, GENERIC_READ );
		if( jobService )
		{
			QueryServiceStatus( jobService, &status );
			crontabStatusLabel->setText( getServiceStatus( status.dwCurrentState ) );
			CloseServiceHandle( jobService );
		}
		else
			crontabStatusLabel->setText( "N/A" );

		invalidateWindow();

		CloseServiceHandle( serviceManager );
	}
}

void CronConfMainWindow::StartService()
{
	// INSERT>> Your code here.
	SC_HANDLE		serviceManager;
	SC_HANDLE		jobService;

	serviceManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( serviceManager )
	{
		// stop the dispatcher
		jobService = OpenService( serviceManager, SERVICE_NAME, SERVICE_ALL_ACCESS );
		if( jobService )
		{
			if( !::StartService( jobService, 0, NULL ) )
				messageBox( "Unable to start job service", "Error", MB_OK|MB_ICONERROR );

			CloseServiceHandle( jobService );
		}
		else
			messageBox( "Unable to open job service", "Error", MB_OK|MB_ICONERROR );

		CloseServiceHandle( serviceManager );
	}
	else
		messageBox( "Unable to open service manager", "FATAL", 0 );

	checkService();
}

void CronConfMainWindow::StopService()
{
	SC_HANDLE		serviceManager;
	SC_HANDLE		jobService;
	SERVICE_STATUS	serviceStatus;

	serviceManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( serviceManager )
	{
		// stop the dispatcher
		jobService = OpenService( serviceManager, SERVICE_NAME, SERVICE_ALL_ACCESS );
		if( jobService )
		{
			// stop the current service
			do
			{
				ControlService( jobService, SERVICE_CONTROL_STOP, &serviceStatus );
				Sleep( 1000 );
				QueryServiceStatus( jobService, &serviceStatus );

			}while( !(serviceStatus.dwCurrentState == SERVICE_STOPPED) );

			CloseServiceHandle( jobService );
		}

		CloseServiceHandle( serviceManager );
	}
	else
		messageBox( "Unable to open service manager", "FATAL", MB_OK|MB_ICONSTOP );

	checkService();
}


void CronConfMainWindow::RemoveService()
{
  // INSERT>> Your code here.
	SC_HANDLE		serviceManager;
	SC_HANDLE		jobService;
	SERVICE_STATUS	serviceStatus;

	serviceManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( serviceManager )
	{
		// stop the dispatcher
		jobService = OpenService( serviceManager, SERVICE_NAME, SERVICE_ALL_ACCESS );
		if( jobService )
		{
			// stop the current service
			do
			{
				ControlService( jobService, SERVICE_CONTROL_STOP, &serviceStatus );
				Sleep( 1000 );
				QueryServiceStatus( jobService, &serviceStatus );

			}while( !(serviceStatus.dwCurrentState == SERVICE_STOPPED) );

			DeleteService( jobService );
			CloseServiceHandle( jobService );
		}

		CloseServiceHandle( serviceManager );
	}
	else
		messageBox( "Unable to open service manager", "FATAL", MB_OK|MB_ICONSTOP );

	checkService();
}


void CronConfMainWindow::InstallService()
{
	// INSERT>> Your code here.
	SC_HANDLE		serviceManager;
	SC_HANDLE		jobService;

	serviceManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if( serviceManager )
	{
		// stop the dispatcher
		jobService = OpenService( serviceManager, SERVICE_NAME, SERVICE_ALL_ACCESS );
		if( !jobService )
		{
			// service not yet created
			// create the service

			STRING appName = cronConfApplication.getFileName();
			size_t lastBackSlash = appName.searchRChar( DIRECTORY_DELIMITER );
			appName.cut( lastBackSlash+1 );

			appName += "CRONTAB.EXE";

			STRING password = EditPassword->getText();
			STRING username = EditUsername->getText();
			if( !username.isEmpty() )
			{
				if( username.searchChar( '\\' ) == STRING::no_index )
					username = STRING(".\\" ) + username;
			}

			jobService = CreateService(
							serviceManager,
							SERVICE_NAME,
							SERVICE_NAME,
							SERVICE_ALL_ACCESS,
							SERVICE_WIN32_OWN_PROCESS,
							SERVICE_AUTO_START,
							SERVICE_ERROR_NORMAL,
							appName,
							NULL, NULL, NULL,
							!username.isEmpty() ? (const char *)username : NULL,
							!username.isEmpty() ? (const char *)password : NULL );
		}

		if( jobService )
			CloseServiceHandle( jobService );
		else
			messageBox( "Unable to create job service", "Error", MB_OK|MB_ICONERROR );

		CloseServiceHandle( serviceManager );
	}
	else
		messageBox( "Unable to open service manager", "FATAL", 0 );

	checkService();
}

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
ProcessStatus CreateEditJobForm::handleCreate()
{
	if( !m_jobTitle.isEmpty() )
	{
		STRING	commandLineBuffer;
		STRING	nextStartBuffer;
		STRING	intervalBuffer;
		STRING	intervalTypeBuffer;
		bool	multipleInst;

		readJob( m_jobTitle, &commandLineBuffer, &nextStartBuffer, &intervalBuffer, &intervalTypeBuffer, &multipleInst );

		newTitle->setText( m_jobTitle );
		nextStart->setText( nextStartBuffer );
		commandLine->setText( commandLineBuffer );
		interval->setText( intervalBuffer  );
		intervalType->selectEntry( intervalTypeBuffer );
		multipleInstances->setActive( multipleInst );
	}
	else
	{
		newTitle->setText( "" );
		nextStart->setText( "1.1.2000 0:0:0" );
		commandLine->setText( "" );
		interval->setText( ""  );
		intervalType->selectEntry( "Seconds" );
	}

	return psDO_DEFAULT;
}

ProcessStatus CreateEditJobForm::handleOk()
{
	bool	doClose = false;

	STRING	commandLineBuffer,
			nextStartBuffer,
			titleBuffer,
			intervalBuffer,
			intervalTypeBuffer;
	int		error;
	bool	multipleInst;

	titleBuffer = newTitle->getText();
	nextStartBuffer = nextStart->getText();
	commandLineBuffer = commandLine->getText();
	intervalBuffer = interval->getText();
	intervalTypeBuffer = intervalType->getText();
	multipleInst = multipleInstances->isActive();

	if( !titleBuffer.isEmpty() )
	{
		error = writeJob( m_jobTitle, titleBuffer, commandLineBuffer, nextStartBuffer, intervalBuffer, intervalTypeBuffer, multipleInst );
		switch( error )
		{
			case JOB_OK:
				doClose = true;
				break;
			case JOB_BAD_DELETE:
				messageBox( "Cannot delete old key", "Error", MB_OK|MB_ICONSTOP );
				break;
			case JOB_BAD_INSERT:
				messageBox( "Cannot create new key", "Error", MB_OK|MB_ICONSTOP );
				break;
			case JOB_EXISTS:
				messageBox( "Job exists already", "Error", MB_OK|MB_ICONSTOP );
				break;
		}
	}
	else
		messageBox( "Enter a job title!", "Error", MB_OK|MB_ICONSTOP );

	if( doClose )
	{
		setModalResult( IDOK );
		close();
	}

	return psPROCESSED;
}

ProcessStatus CronConfMainWindow::handleCreate()
{
	InfoLabel->setText(gak::formatNumber(sizeof(void*)*8) + "-bit");

	readCronJobs( cronJobs );
	checkService();
	return psDO_DEFAULT;
}

ProcessStatus CronConfMainWindow::handleButtonClick( int control )
{
	switch( control )
	{
		case startButton_id:
			StartService();
			break;
		case stopButton_id:
			StopService();
			break;
		case installButton_id:
			InstallService();
			break;
		case removeButton_id:
			RemoveService();
			break;


		case AddButton_id:
			AddCronJob();
			break;

		case EditButton_id:
			EditCronJob();
			break;

		case DeleteButton_id:
			DeleteJob();
			break;
		
			
		default:
			return cronConfMainForm_form::handleButtonClick( control );
	}
	return psPROCESSED;
}
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

