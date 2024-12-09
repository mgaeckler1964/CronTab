/*
	this file was created from C:\CRESD\Source\CronTab\CronConf.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/

#include "C:\CRESD\Source\CronTab\CronConf.gui.h"

namespace winlibGUI {

	void cronConfMainForm_form::getControls() {
		startButton=static_cast<winlib::PushButton*>(findChild(startButton_id));
		stopButton=static_cast<winlib::PushButton*>(findChild(stopButton_id));
		installButton=static_cast<winlib::PushButton*>(findChild(installButton_id));
		removeButton=static_cast<winlib::PushButton*>(findChild(removeButton_id));
		EditUsername=static_cast<winlib::EditControl*>(findChild(EditUsername_id));
		EditPassword=static_cast<winlib::EditControl*>(findChild(EditPassword_id));
		crontabStatusLabel=static_cast<winlib::Label*>(findChild(crontabStatusLabel_id));
		AddButton=static_cast<winlib::PushButton*>(findChild(AddButton_id));
		DeleteButton=static_cast<winlib::PushButton*>(findChild(DeleteButton_id));
		EditButton=static_cast<winlib::PushButton*>(findChild(EditButton_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		InfoLabel=static_cast<winlib::Label*>(findChild(InfoLabel_id));
		cronJobs=static_cast<winlib::ListBox*>(findChild(cronJobs_id));
	}	// cronConfMainForm

	void CreateEditJobForm_form::getControls() {
		newTitle=static_cast<winlib::EditControl*>(findChild(newTitle_id));
		commandLine=static_cast<winlib::EditControl*>(findChild(commandLine_id));
		nextStart=static_cast<winlib::EditControl*>(findChild(nextStart_id));
		interval=static_cast<winlib::EditControl*>(findChild(interval_id));
		intervalType=static_cast<winlib::ComboBox*>(findChild(intervalType_id));
		multipleInstances=static_cast<winlib::CheckBox*>(findChild(multipleInstances_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		IDCANCEL_PushButton=static_cast<winlib::PushButton*>(findChild(IDCANCEL));
	}	// CreateEditJobForm
}	// namespace winlibGUI
