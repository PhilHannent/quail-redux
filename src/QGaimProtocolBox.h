#ifndef _QGAIM_PROTOCOL_BOX_H_
#define _QGAIM_PROTOCOL_BOX_H_

#include <qvariant.h>
#include <qcombobox.h>

#include <libgaim/prpl.h>

class QGaimProtocolBox : public QComboBox
{
	Q_OBJECT

	public:
		QGaimProtocolBox(QWidget *parent = NULL, const char *name = NULL);
		QGaimProtocolBox(GaimProtocol protocol, QWidget *parent = NULL,
						 const char *name = NULL);

		void setCurrentProtocol(GaimProtocol protocol);

	protected:
		void buildMenu(GaimProtocol protocol);
};

#endif /* _QGAIM_PROTOCOL_BOX_H_ */
