/*
 *
 *
 */

#ifndef QWEBVDOM_H
#define QWEBVDOM_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtGui/qicon.h>
#include <QtScript/qscriptengine.h>
#if QT_VERSION >= 0x040400
#include <QtNetwork/qnetworkaccessmanager.h>
#endif

#include "qwebkitglobal.h"

#include <list>
#include "vdom.pb.h"

class QWebFrame;
class QWebVDomPrivate;

class QWEBKIT_EXPORT QWebVDom : public QObject
{
    //Q_OBJECT
public:
    QWebVDom(QWebFrame* qframe);
    ~QWebVDom();

    bool buildVdom(vdom::Window *vdom_win);

private:
    QWebFrame *m_qframe;

};

#endif // QWEBVDOM_H
