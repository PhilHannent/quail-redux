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
class QGridLayout;
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

		void buildLoginOpts(QGridLayout *grid, QWidget *parent, int &row);
		void buildUserOpts(QGridLayout *grid, QWidget *parent, int &row);

	protected slots:
		void proxyTypeChanged(int index);

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

		QCheckBox *mailNotificationCheck;

		/* Proxy tab */
		QComboBox *proxyDropDown;
		QLineEdit *proxyHost;
		QLineEdit *proxyPort;
		QLineEdit *proxyUsername;
		QLineEdit *proxyPassword;
};

#endif /* _QGAIM_ACCOUNT_EDITOR_H_ */
