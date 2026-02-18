/*
	this file was created from C:\CRESD\Source\CronTab\CronConf.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/

#include "C:\CRESD\Source\CronTab\CronConf.gui.h"

namespace winlibGUI {

	gak::xml::Document *GuiApplication::getGuiDoc() {
		gak::xml::Document *doc = winlib::Application::getGuiDoc();
		if(!doc) {
			gak::STRING xmlSrc = "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n"
"<gui>\n"
"<forms>\n"
"<form name=\"cronConfMainForm\" width=\"478\" height=\"607\" caption=\"GAK Contab Configuration\" id=\"128\" baseClass=\"PopupWindow\" style=\"277807104\">\n"
"<child type=\"FrameChild\" name=\"\" caption=\"\" x=\"106\" y=\"115\" width=\"80\" height=\"160\" style=\"1342308352\">\n"
"<child type=\"FrameChild\" name=\"\" caption=\"\" x=\"60\" y=\"108\" width=\"80\" height=\"50\" style=\"1342308352\">\n"
"<layoutManager type=\"RowManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"PushButton\" name=\"startButton\" caption=\"Start\" x=\"8\" y=\"9\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"130\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"stopButton\" caption=\"Stop\" x=\"97\" y=\"9\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"131\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"installButton\" caption=\"Install\" x=\"187\" y=\"8\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"132\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"removeButton\" caption=\"Remove\" x=\"281\" y=\"9\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"133\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<layout attachment=\"1\" growWidth=\"1\" />\n"
"</child>\n"
"<layout attachment=\"1\" growWidth=\"1\" />\n"
"<layoutManager type=\"AttachmentManager\" />\n"
"<child type=\"FrameChild\" name=\"\" caption=\"\" x=\"0\" y=\"80\" width=\"80\" height=\"100\" style=\"1342308352\">\n"
"<layout attachment=\"3\" growWidth=\"1\" />\n"
"<child type=\"Label\" name=\"\" caption=\"Username:\" x=\"18\" y=\"18\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout x=\"0\" y=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Crontab Service:\" x=\"8\" y=\"57\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout x=\"0\" y=\"2\" width=\"1\" height=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Password:\" x=\"26\" y=\"54\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout x=\"0\" y=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"EditUsername\" caption=\"\" x=\"229\" y=\"22\" width=\"80\" height=\"25\" style=\"1350631808\" id=\"134\">\n"
"<layout x=\"1\" y=\"0\" growWidth=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"EditPassword\" caption=\"\" x=\"239\" y=\"65\" width=\"80\" height=\"25\" style=\"1350631584\" id=\"135\">\n"
"<layout x=\"1\" y=\"1\" growWidth=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"TableManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"Label\" name=\"crontabStatusLabel\" caption=\"status\" x=\"150\" y=\"57\" width=\"200\" height=\"20\" style=\"1342242827\" id=\"136\">\n"
"<layout x=\"1\" y=\"2\" width=\"1\" height=\"1\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"</child>\n"
"</child>\n"
"<child type=\"FrameChild\" name=\"\" caption=\"\" x=\"131\" y=\"210\" width=\"80\" height=\"41\" style=\"1342308352\">\n"
"<layout attachment=\"3\" growWidth=\"1\" />\n"
"<child type=\"PushButton\" name=\"AddButton\" caption=\"Add...\" x=\"23\" y=\"38\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"137\">\n"
"<layout attachment=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"DeleteButton\" caption=\"Delete...\" x=\"96\" y=\"8\" width=\"80\" height=\"24\" style=\"1342242816\" id=\"138\">\n"
"<layout attachment=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"EditButton\" caption=\"Edit...\" x=\"184\" y=\"8\" width=\"80\" height=\"24\" style=\"1342242816\" id=\"139\">\n"
"<layout attachment=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"Close\" x=\"510\" y=\"57\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"1\">\n"
"<layout attachment=\"2\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"<layoutManager type=\"AttachmentManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"Label\" name=\"InfoLabel\" caption=\"INF\" x=\"288\" y=\"16\" width=\"80\" height=\"24\" style=\"1342242827\" id=\"140\">\n"
"<layout attachment=\"0\" paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" />\n"
"</child>\n"
"</child>\n"
"<layoutManager type=\"AttachmentManager\" marginLeft=\"0\" marginRight=\"0\" marginTop=\"0\" marginBottom=\"0\" />\n"
"<child type=\"Label\" name=\"\" caption=\"CRON Jobs:\" x=\"57\" y=\"193\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout attachment=\"1\" paddingLeft=\"8\" paddingRight=\"8\" paddingTop=\"10\" paddingBottom=\"4\" />\n"
"</child>\n"
"<child type=\"ListBox\" name=\"cronJobs\" caption=\"\" x=\"103\" y=\"258\" width=\"80\" height=\"25\" style=\"1350631681\" id=\"141\">\n"
"<layout attachment=\"4\" paddingLeft=\"8\" paddingRight=\"8\" paddingTop=\"4\" paddingBottom=\"0\" growWidth=\"1\" growHeight=\"1\" />\n"
"</child>\n"
"</form>\n"
"<form name=\"CreateEditJobForm\" width=\"634\" height=\"172\" caption=\"Create/Edit Cron Job\" style=\"2424569856\" baseClass=\"ModalPopup\" id=\"129\">\n"
"<layoutManager type=\"TableManager\" marginLeft=\"4\" marginRight=\"4\" marginTop=\"4\" marginBottom=\"4\" />\n"
"<child type=\"Label\" name=\"\" caption=\"Title:\" x=\"25\" y=\"21\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"1\" y=\"0\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Commandline:\" x=\"30\" y=\"48\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"1\" y=\"1\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Next Start:\" x=\"32\" y=\"71\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"1\" y=\"2\" />\n"
"</child>\n"
"<child type=\"Label\" name=\"\" caption=\"Interval:\" x=\"31\" y=\"104\" width=\"0\" height=\"0\" style=\"1342242827\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"1\" y=\"3\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"newTitle\" caption=\"\" x=\"119\" y=\"26\" width=\"80\" height=\"25\" style=\"1350631808\" id=\"142\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"2\" y=\"0\" growWidth=\"1\" width=\"3\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"commandLine\" caption=\"\" x=\"125\" y=\"62\" width=\"80\" height=\"25\" style=\"1350631808\" id=\"143\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"2\" y=\"1\" growWidth=\"1\" width=\"3\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"nextStart\" caption=\"\" x=\"136\" y=\"101\" width=\"80\" height=\"25\" style=\"1350631808\" id=\"144\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"2\" y=\"2\" growWidth=\"1\" width=\"3\" />\n"
"</child>\n"
"<child type=\"EditControl\" name=\"interval\" caption=\"\" x=\"131\" y=\"144\" width=\"80\" height=\"25\" style=\"1350631808\" id=\"145\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"2\" y=\"3\" />\n"
"</child>\n"
"<child type=\"ComboBox\" name=\"intervalType\" caption=\"\" x=\"274\" y=\"156\" width=\"80\" height=\"200\" style=\"1342243843\" id=\"146\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"3\" y=\"3\" />\n"
"<items>\n"
"<item>Seconds</item>\n"
"<item>Minutes</item>\n"
"<item>Hours</item>\n"
"<item>Days</item>\n"
"</items>\n"
"</child>\n"
"<child type=\"CheckBox\" name=\"multipleInstances\" caption=\"Multiple Instances\" x=\"385\" y=\"159\" width=\"0\" height=\"0\" style=\"1342242819\" id=\"147\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"4\" y=\"3\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"596\" y=\"28\" width=\"80\" height=\"25\" style=\"1342242817\" id=\"1\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"5\" y=\"0\" />\n"
"</child>\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"&amp;Cancel\" x=\"605\" y=\"81\" width=\"80\" height=\"25\" style=\"1342242816\" id=\"2\">\n"
"<layout paddingLeft=\"4\" paddingRight=\"4\" paddingTop=\"4\" paddingBottom=\"4\" x=\"5\" y=\"1\" />\n"
"</child>\n"
"</form>\n"
"</forms>\n"
"</gui>";
			return Application::getGuiDoc( xmlSrc );
		}
		return doc;
	}


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
