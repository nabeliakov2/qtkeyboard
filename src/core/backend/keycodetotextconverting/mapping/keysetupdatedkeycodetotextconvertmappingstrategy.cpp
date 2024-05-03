#include "core/backend/keycodetotextconverting/mapping/keysetupdatedkeycodetotextconvertmappingstrategy.h"

#include <QtCore/QSharedPointer>
#include <QtCore/QPointer>

#include "core/backend/base/abstractkeyboardbackend.h"
#include "core/backend/keysets/keyboardkeyset.h"

namespace qt_keyboard
{
void KeySetUpdatedKeycodeToTextConvertMappingStrategy::updateLayouts(const KeyboardKeySet *const _pKeySet,
                                                                     const QStringList &_layoutList,
                                                                     const KeycodeToTextConvertStrategyInterface *const _pKeyTextSrc) // public
{
    if (_pKeySet && _pKeyTextSrc)
    {
        if (_layoutList.isEmpty())
        {
            clear();
        }
        else
        {
            reset(_layoutList.count());
            auto checkAndSetText = [](QString &_textToReturn,
                                      const KeycodeToTextConvertStrategyInterface *const _pKetText,
                                      const int _keycode, const int _layoutIndex, const int _shifted) -> bool
            {
                if (_pKetText -> hasKeyText(_keycode, _layoutIndex, _shifted))
                {
                    _textToReturn = _pKetText -> keyText(_keycode, _layoutIndex, _shifted);
                    return true;
                }
                return false;
            };


            for (int layoutIndex = 0; layoutIndex < _layoutList.size(); ++layoutIndex)
            {
                doSetKeyText(KeyboardUtility::SwitchLayout, _layoutList.at(layoutIndex), layoutIndex);
                doSetKeyText(KeyboardUtility::SwitchLayout, _layoutList.at(layoutIndex), layoutIndex, true);
                const auto &keySet(_pKeySet -> keySet());
                for (int i = 0; i < keySet.size(); ++i)
                {
                    const auto &keyRow(keySet.at(i));
                    for (int j = 0; j < keyRow.size(); ++j)
                    {
                        const int currKeycode = keyRow.at(j);
                        QString currText;
                        QString currShiftedText;
                        if (checkAndSetText(currText, _pKeyTextSrc, currKeycode, layoutIndex, false))
                        {
                            doSetKeyText(currKeycode, currText, layoutIndex, false);
                        }
                        if (checkAndSetText(currShiftedText, _pKeyTextSrc, currKeycode, layoutIndex, true))
                        {
                            doSetKeyText(currKeycode, currShiftedText, layoutIndex, true);
                        }
                    }  // -> for j
                }   // -> for i
                ++layoutIndex;
            }
        }
    }
}








} // qt_keyboard
