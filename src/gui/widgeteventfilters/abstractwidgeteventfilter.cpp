#include "gui/widgeteventfilters/abstractwidgeteventfilter.h"

namespace qt_keyboard
{


void AbstractWidgetEventFilter::setWidget(QWidget *const _pWidget)  // public
{
    if (_pWidget != pWidget.data())
    {
        unsetWidget();

        if (_pWidget)
        {
            pWidget = _pWidget;
            pWidget -> installEventFilter(this);
        }
    }
}


void AbstractWidgetEventFilter::unsetWidget()   // public
{
    if (pWidget)
    {
        pWidget -> removeEventFilter(this);
        pWidget.clear();
    }
}


} // qt_keyboard
