#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QDebug>
#include <QUrl>
#include <QApplication>

#include "vdom.h"
#include "qwebvdom.h"

using namespace vdom;
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QWebView* view = new QWebView(NULL);
    QWebPage* page = view->page();

    QEventLoop loop;
    QUrl url("http://www.baidu.com");
    if (argc > 1) {
        url = QUrl(argv[1]);
    }

    view->load(url);
    QObject::connect(page, SIGNAL(loadFinished(bool)), &loop,
SLOT(quit()));
    loop.exec();
    QWebFrame * frame = view->page()->mainFrame();
    QWebVDom *webvdom = new QWebVDom(frame);
    ::vdom::Window window;
    webvdom->buildVdom(&window);
    std::cout << window.DebugString();
    view->show();
    qDebug() << "web frame\n";
    app.exec();
}
