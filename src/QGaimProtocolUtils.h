#ifndef _QGAIM_PROTOCOL_UTILS_H_
#define _QGAIM_PROTOCOL_UTILS_H_

#include <qvariant.h>

#include <libgaim/account.h>
#include <libgaim/plugin.h>
#include <libgaim/prpl.h>

class QPixmap;

class QGaimProtocolUtils
{
	public:
		static QString getProtocolName(GaimProtocol protocol);
		static QPixmap *getProtocolIcon(GaimAccount *account);
		static QPixmap *getProtocolIcon(const GaimPlugin *plugin);

	protected:
		static QPixmap *getProtocolIcon(const QString &protoName);
};

#endif /* _QGAIM_PROTOCOL_UTILS_H_ */
