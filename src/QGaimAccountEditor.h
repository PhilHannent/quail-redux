#ifndef _QGAIM_ACCOUNT_EDITOR_H_
#define _QGAIM_ACCOUNT_EDITOR_H_

#include <libgaim/account.h>
#include <libgaim/plugin.h>
#include <libgaim/prpl.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qmainwindow.h>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPixmap;
class QTabWidget;

class QGaimAccountEditor : public QMainWindow
{
	Q_OBJECT

	public:
		QGaimAccountEditor(GaimAccount *account = NULL);
		~QGaimAccountEditor();

	protected:
		virtual void buildInterface();
		QWidget *buildAccountTab();
		QWidget *buildProtocolTab();
		QWidget *buildProxyTab();

		QWidget *buildLoginOpts(QWidget *parent);
		QWidget *buildUserOpts(QWidget *parent);

	private:
		GaimAccount *account;
		GaimPlugin *plugin;
		GaimPluginProtocolInfo *prplInfo;

		QString protocolId;
		GaimProtocol protocol;

		GList *userSplitEntries;
		GList *protocolOptEntries;

		GaimProxyType newProxyType;

		QTabWidget *tabs;

		/* Account tab */
		QComboBox *protocolList;
		QLineEdit *screenNameEntry;
		QLabel *passwordLabel;
		QLineEdit *passwordEntry;
		QLineEdit *aliasEntry;
		QCheckBox *rememberPassCheck;
		QCheckBox *autoLoginCheck;
};

#endif /* _QGAIM_ACCOUNT_EDITOR_H_ */
