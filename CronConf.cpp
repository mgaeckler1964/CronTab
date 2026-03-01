/*
		Project:		Crontab
		Module:			CronConf.cpp
		Description:	The configuration utility for my cron tab
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2026 Martin Gäckler

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

/**
	TODO
	====

	After migrating contab to C++ and the Registry class we shoulf implement 
	a migration from some string values to their integer representation.
	these are: multiple, interval and intervaltype

*/
// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/fmtNumber.h>
#include <gak/array.h>

#include <winlib/WINAPP.H>
#include <winlib/registry.h>

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

using namespace gak;
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
	long			openResult;
	Registry		softKey;
	Registry		cresdKey;
	Registry		crontabKey;
	Registry		jobKey;
	unsigned char	multiple[16];
	
	int				result = JOB_BAD_READ;

	openResult = softKey.openPublic(SOFTWARE_KEY, KEY_READ|KEY_WOW64_32KEY );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = cresdKey.openSubkey( softKey, COMPANY, KEY_READ|KEY_WOW64_32KEY );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = crontabKey.openSubkey( cresdKey, SERVICE_NAME, KEY_READ|KEY_WOW64_32KEY );
			if( openResult == ERROR_SUCCESS )
			{
				openResult = jobKey.openSubkey(	crontabKey, jobTitle, KEY_READ|KEY_WOW64_32KEY );
				if( openResult == ERROR_SUCCESS )
				{
					if( jobKey.readValue( COMMAND_LINE, command ) != rsOK )
						command->release();

					if( jobKey.readValue( INTERVAL, interval ) != rsOK )
						interval->release();

					if( jobKey.readValue( INTERVAL_TYPE, intervalType ) != rsOK )
						intervalType->release();

					if( jobKey.readValue( NEXT_START, nextStart ) != rsOK )
						nextStart->release();

					size_t size = sizeof( multiple );
					if( jobKey.queryValue( MULTIPLE_INST, &multiple, &size ) == rtSTRING && multiple[0U] == '1' )
					{
						*multipleInst = true;
					}
					else
					{
						*multipleInst = false;
					}

					result = JOB_OK;
				}
				else
					result = JOB_BAD_INSERT;
			}
		}
	}

	return result;
}

static int writeJob(	const STRING &oldJob, const STRING &newTitle,
						const STRING &command,
						const STRING &nextStart,
						const STRING &interval,
						const STRING &intervalType,
						bool		multipleInstances )
{
	long			openResult;
	Registry		softKey;

	int				result = JOB_BAD_INSERT;

	openResult = softKey.openPublic( SOFTWARE_KEY, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
	if( openResult == ERROR_SUCCESS )
	{
		Registry		cresdKey;

		openResult = cresdKey.createKey( softKey, COMPANY, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
		if( openResult == ERROR_SUCCESS )
		{
			Registry		crontabKey;

			openResult = crontabKey.createKey( cresdKey, SERVICE_NAME, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
			if( openResult == ERROR_SUCCESS )
			{
				Registry		jobKey;

				if( oldJob != newTitle )
				{
					openResult = jobKey.openSubkey( crontabKey, newTitle, KEY_READ|KEY_WOW64_32KEY );
					if( openResult == ERROR_SUCCESS )
					{
						result = JOB_EXISTS;
					}
				}
				if( !jobKey )
				{
					char	multipleInst[2];

					openResult = ERROR_SUCCESS;
					if( !oldJob.isEmpty() )
					{
						openResult = crontabKey.deleteSubkey( oldJob );
						if( openResult != ERROR_SUCCESS )
							result = JOB_BAD_DELETE;
					}
					if( openResult == ERROR_SUCCESS )
					{
						openResult = jobKey.createKey( crontabKey, newTitle, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
						if( openResult == ERROR_SUCCESS )
						{
							jobKey.writeValue( COMMAND_LINE, command );
							jobKey.writeValue( INTERVAL, interval );
							jobKey.writeValue( INTERVAL_TYPE, intervalType );
							jobKey.writeValue( NEXT_START, nextStart );

							multipleInst[0] = multipleInstances ? '1' : '0';
							multipleInst[1] = 0;
							jobKey.setValueEx( MULTIPLE_INST, rtSTRING, multipleInst, 2 );

							result = JOB_OK;
						}
						else
							result = JOB_BAD_INSERT;
					}
				}
			}
		}
	}

	return result;
}

static void deleteCronJob( const char *jobTitle )
{
	long			openResult;
	Registry		softKey;
	Registry		cresdKey;
	Registry		crontabKey;

	openResult = softKey.openPublic( SOFTWARE_KEY, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = cresdKey.createKey( softKey, COMPANY, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = crontabKey.createKey( cresdKey, SERVICE_NAME, KEY_ALL_ACCESS|KEY_WOW64_32KEY );
			if( openResult == ERROR_SUCCESS )
			{
				crontabKey.deleteSubkey( jobTitle );
			}
		}
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
	long			openResult;
	Registry		softKey;
	Registry		cresdKey;
	Registry		crontabKey;

	cronJobs->clearEntries();

	openResult = softKey.openPublic( SOFTWARE_KEY, KEY_READ|KEY_WOW64_32KEY );
	if( openResult == ERROR_SUCCESS )
	{
		openResult = cresdKey.openSubkey( softKey, COMPANY, KEY_READ|KEY_WOW64_32KEY );
		if( openResult == ERROR_SUCCESS )
		{
			openResult = crontabKey.openSubkey(	cresdKey, SERVICE_NAME, KEY_READ|KEY_WOW64_32KEY );
			if( openResult == ERROR_SUCCESS )
			{
				for(
					Registry::key_iterator it = crontabKey.kbegin(), endIT = crontabKey.kend();
					it != endIT;
					++it
				)
				{
					cronJobs->addEntry( *it );
				}
			}
		}
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

	int		error;

	STRING title = newTitle->getText();
	if( !title.isEmpty() )
	{
		error = writeJob( 
			m_jobTitle, 
			title, commandLine->getText(), 
			nextStart->getText(), interval->getText(), intervalType->getText(), 
			multipleInstances->isActive() 
		);
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

