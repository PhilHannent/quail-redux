#ifndef _QGAIM_ACCOUNT_EDITOR_H_
#define _QGAIM_ACCOUNT_EDITOR_H_

#include <libgaim/account.h>
#include <libgaim/plugin.h>
#include <libgaim/prpl.h>

#include <qdialog.h>
#include <qvariant.h>
#include <qwidget.h>

class QCheckBox;
class QComboBox;
class QGaimProtocolBox;
class QGridLayout;
class QLabel;
class QLineEdit;
class QPixmap;
class QTabWidget;

class QGaimAccountEditor : public QDialog
{
	Q_OBJECT

	public:
		QGaimAccountEditor(GaimAccount *account = NULL,
						   QWidget *parent = NULL, const char *name = NULL,
						   WFlags fl = 0);
		~QGaimAccountEditor();

	protected:
		virtual void buildInterface();
		QWidget *buildAccountTab();
		QWidget *buildProtocolTab();
		QWidget *buildProxyTab();

		void buildLoginOpts(QGridLayout *grid, QWidget *parent, int &row);
		void buildUserOpts(QGridLayout *grid, QWidget *parent, int &row);

		void updateAccountTab();
		void updateProtocolTab();

	protected slots:
		void proxyTypeChanged(int index);
		void protocolChanged(int index);

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
		QGaimProtocolBox *protocolList;
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
