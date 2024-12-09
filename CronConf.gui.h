/*
	this file was created from C:\CRESD\Source\CronTab\CronConf.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/
#ifndef C__CRESD_SOURCE_CRONTAB_CRONCONF_GUI_H
#define C__CRESD_SOURCE_CRONTAB_CRONCONF_GUI_H
#include <winlib/popup.h>
#include <winlib/frame.h>
#include <winlib/scrollFrame.h>
#include <winlib/ControlW.h>
#include <winlib/xmlEditorChild.h>
#include <winlib/gridView.h>

namespace winlibGUI {

	const int AddButton_id=137;
	const int commandLine_id=143;
	const int CreateEditJobForm_id=129;
	const int cronConfMainForm_id=128;
	const int cronJobs_id=141;
	const int crontabStatusLabel_id=136;
	const int DeleteButton_id=138;
	const int EditButton_id=139;
	const int EditPassword_id=135;
	const int EditUsername_id=134;
	const int InfoLabel_id=140;
	const int installButton_id=132;
	const int interval_id=145;
	const int intervalType_id=146;
	const int multipleInstances_id=147;
	const int newTitle_id=142;
	const int nextStart_id=144;
	const int removeButton_id=133;
	const int startButton_id=130;
	const int stopButton_id=131;

	class cronConfMainForm_form : public winlib::PopupWindow {
		public:
		cronConfMainForm_form(winlib::BasicWindow *owner) : PopupWindow(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return PopupWindow::create(parent,"cronConfMainForm");
		}

		winlib::PushButton *startButton;
		winlib::PushButton *stopButton;
		winlib::PushButton *installButton;
		winlib::PushButton *removeButton;
		winlib::EditControl *EditUsername;
		winlib::EditControl *EditPassword;
		winlib::Label *crontabStatusLabel;
		winlib::PushButton *AddButton;
		winlib::PushButton *DeleteButton;
		winlib::PushButton *EditButton;
		winlib::PushButton *IDOK_PushButton;
		winlib::Label *InfoLabel;
		winlib::ListBox *cronJobs;
		private:
		virtual void getControls();
	};	// end of cronConfMainForm

	class CreateEditJobForm_form : public winlib::ModalPopup {
		public:
		CreateEditJobForm_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"CreateEditJobForm");
		}

		winlib::EditControl *newTitle;
		winlib::EditControl *commandLine;
		winlib::EditControl *nextStart;
		winlib::EditControl *interval;
		winlib::ComboBox *intervalType;
		winlib::CheckBox *multipleInstances;
		winlib::PushButton *IDOK_PushButton;
		winlib::PushButton *IDCANCEL_PushButton;
		private:
		virtual void getControls();
	};	// end of CreateEditJobForm
}	// namespace winlibGUI

#endif // C__CRESD_SOURCE_CRONTAB_CRONCONF_GUI_H
