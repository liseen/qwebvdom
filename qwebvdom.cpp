/* qvdom.
 * get vdom string from QWebFrame
 */

#include "config.h"
#include "CSSComputedStyleDeclaration.h"
#include "CSSMutableStyleDeclaration.h"
#include "CSSParser.h"
#include "CSSRule.h"
#include "CSSRuleList.h"
#include "CSSStyleRule.h"
#include "CSSComputedStyleDeclaration.h"
#include "qwebframe.h"
#include "qwebframe_p.h"
#include "Frame.h"
#include "Node.h"
#include "NodeList.h"
#include "Element.h"
#include "Document.h"
#include "HTMLDocument.h"
#include "DOMWindow.h"
#include "HTMLElement.h"
#include "HTMLLinkElement.h"
#include "HTMLInputElement.h"
#include "HTMLImageElement.h"
#include "HTMLFrameOwnerElement.h"
#include "RenderView.h"
#include "Location.h"
#include "wtf/HashMap.h"
#include "HTMLNames.h"
#include "RenderText.h"
#include "RenderTableCell.h"
#include "HTMLParser.h"
#include "IntRect.h"
#include "Frame.h"
#include "Element.h"
#include "Document.h"
#include "DOMWindow.h"
#include "Location.h"
#include "wtf/HashMap.h"
#include "HTMLElement.h"
#include "HTMLDocument.h"
#include <wtf/RefCountedLeakCounter.h>
#include "InlineTextBox.h"
#include "CString.h"

#include "qwebvdom.h"

using namespace WebCore;

static void dumpVWindow(vdom::Window* vdom_win, DOMWindow* win);
static void dumpVChildren(vdom::Node* vdom_elem, HTMLElement* elem);
static void dumpVDocument(vdom::Document* vdom_doc, HTMLDocument* doc);
static void dumpVElement(vdom::Node* vdom_elem, HTMLElement* elem);
static void dumpVChildren(vdom::Node* vdom_elem, HTMLElement* elem);
static void dumpVTextNode(vdom::Node* vdom_text, Node* node);

static void dumpVWindow(vdom::Window* vdom_win, DOMWindow* win) {
    vdom_win->set_location(win->location()->href().utf8().data());
    vdom_win->set_inner_width(win->innerWidth());
    vdom_win->set_outer_width(win->outerWidth());
    vdom_win->set_inner_height(win->innerHeight());
    vdom_win->set_outer_height(win->outerHeight());

    Document* doc = win->document();
    if (doc->isHTMLDocument()) {
        dumpVDocument(vdom_win->mutable_doc(), static_cast<HTMLDocument*>(doc));
    }
}

static void dumpVDocument(vdom::Document* vdom_doc, HTMLDocument* doc) {
    vdom_doc->set_width(doc->width());
    vdom_doc->set_height(doc->height());
    vdom_doc->set_title(doc->title().utf8().data());

    dumpVElement(vdom_doc->mutable_body(), static_cast<HTMLElement*>(doc->body()));
}

static void dumpVElement(vdom::Node* vdom_elem, HTMLElement* elem) {
    if (!elem) {
        return;
    }

    vdom_elem->set_type(vdom::Node::ELEMENT);
    RefPtr<CSSComputedStyleDeclaration> style = computedStyle(elem);
    if (style->getPropertyValue(cssPropertyID("display")) == "none") {
        return;
    }

    std::string tag_name(elem->tagName().utf8().data());
    vdom_elem->set_id(elem->getAttribute("id").string().utf8().data());
    vdom_elem->set_name(elem->getAttribute("name").string().utf8().data());
    vdom_elem->set_tag_name(tag_name);
    vdom_elem->set_class_name(elem->getAttribute("class").string().utf8().data());

    FloatPoint absPos = static_cast<RenderObject*>(elem->renderer())->localToAbsolute();
    int posX = absPos.x();
    int posY = absPos.y();

    vdom_elem->set_x(posX);
    vdom_elem->set_y(posY);
    vdom_elem->set_w(elem->offsetWidth());
    vdom_elem->set_h(elem->offsetHeight());

    int propId = cssPropertyID("font-family");
    vdom_elem->set_font_family(style->getPropertyValue(propId).utf8().data());

    propId = cssPropertyID("font-size");
    vdom_elem->set_font_size(style->getPropertyValue(propId).utf8().data());

    propId = cssPropertyID("font-style");
    vdom_elem->set_font_style(style->getPropertyValue(propId).utf8().data());

    propId = cssPropertyID("font-weight");
    vdom_elem->set_font_weight(style->getPropertyValue(propId).utf8().data());

    propId = cssPropertyID("color");
    vdom_elem->set_color(style->getPropertyValue(propId).utf8().data());

    propId = cssPropertyID("background-color");
    vdom_elem->set_background_color(style->getPropertyValue(propId).utf8().data());


    if (tag_name == "A") {
        HTMLLinkElement* linkElem = static_cast<HTMLLinkElement*>(elem);
        if (!linkElem->href().isNull()) {
            vdom_elem->set_href(linkElem->href().string().utf8().data());
        }
    } else if (tag_name == "IMG") {
        HTMLImageElement* imageElem = static_cast<HTMLImageElement*>(elem);
        if (!imageElem->alt().isNull()) {
            vdom_elem->set_alt(imageElem->alt().string().utf8().data());
        }
        if (!imageElem->src().isNull()) {
            vdom_elem->set_src(imageElem->src().string().utf8().data());
        }
    } else if (tag_name == "INPUT") {
        HTMLInputElement* inputElem = static_cast<HTMLInputElement*>(elem);
        if (!inputElem->value().isNull()) {
            vdom_elem->set_value(inputElem->value().utf8().data());
        }
    }

    if (tag_name == "FRAME" || tag_name == "IFRAME") {
        /*
        HTMLFrameOwnerElement* frameElem = static_cast<HTMLFrameOwnerElement*>(elem);
        if (frameElem && frameElem->contentWindow()) {
            dumpVWindow(frameElem->contentWindow());
        }
        */
    } else {
        dumpVChildren(vdom_elem, elem);
    }
}

static void dumpVChildren(vdom::Node* vdom_elem, HTMLElement* elem) {
    RefPtr<NodeList> children = elem->childNodes();
    unsigned len = children->length();
    for (unsigned i = 0; i < len; i++) {
        Node* child = children->item(i);
        if (child->isElementNode()) {
            HTMLElement* child_elem = static_cast<HTMLElement*>(child);
            String tagName = child_elem->tagName();
            if (tagName == "SCRIPT"
                    || tagName == "STYLE"
                    || tagName == "NOSCRIPT"
                    || tagName == "LINK") {
                continue;
            }

            vdom::Node* node = vdom_elem->add_child_nodes();
            dumpVElement(node, child_elem);
        } else if (child->isTextNode()) {
            vdom::Node* node = vdom_elem->add_child_nodes();
            dumpVTextNode(node , child);
        } else { /* ignore */
        }
    }
}

static void dumpVTextNode(vdom::Node* vdom_text, Node* node) {
    vdom_text->set_type(vdom::Node::TEXT);
    vdom_text->set_text(node->nodeValue().utf8().data());

    RenderText* text = static_cast<RenderText*>(node->renderer());
    if (text) {
        FloatPoint absPos = text->localToAbsolute();
        int posX = absPos.x();
        int posY = absPos.y();

        IntRect rect = text->linesBoundingBox();


        vdom_text->set_x(rect.x() + posX);
        vdom_text->set_y(rect.y() + posY);
        vdom_text->set_w(rect.width());
        vdom_text->set_h(rect.height());
    }
}

QWebVDom::QWebVDom(QWebFrame* qframe) {
    m_qframe = qframe;
}

bool QWebVDom::buildVdom(vdom::Window *vdom_win) {
    Frame* frame = QWebFramePrivate::core(m_qframe);
    DOMWindow* win = frame->domWindow();

    dumpVWindow(vdom_win, win);

    return true;
}

QWebVDom::~QWebVDom() {
}


