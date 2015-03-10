#include "Bindings.h"
#include <math.h>
#include <QVariant>

/**
 * @brief Binds a checkbox to a value
 * @param checkbox The box in the UI
 * @param value The value pointer (to a bool)
 * @return A binding between the two
 */
BoolBinding* BoolBinding::bindCheckbox(QCheckBox *checkbox, bool &value) {
    // Bind the checkbox and the value together
    BoolBinding *binding = new BoolBinding(value);
    connect(checkbox, SIGNAL(toggled(bool)), binding, SLOT(boolChanged(bool)));

    // Set the initial value
    checkbox->setChecked(value);

    return binding;
}

/**
 * @brief Binds a dock widget to a value
 * @param dock The dock in the UI
 * @param value The value pointer (to a bool)
 * @return A binding between the two
 */
BoolBinding* BoolBinding::bindDock(QDockWidget *dock, bool &value) {
    // Bind the checkbox and the value together
    BoolBinding *binding = new BoolBinding(value);
    connect(dock, SIGNAL(visibilityChanged(bool)), binding, SLOT(boolChanged(bool)));

    // Set the initial value
    dock->setVisible(value);

    return binding;
}

/**
 * @brief Method to be called when a boolean changed
 * @param newValue the new boolean
 */
void BoolBinding::boolChanged(bool newValue) {
    m_value = newValue;
    emit dataChanged();
}

/**
 * @brief Binds radio buttons to values
 * @param numRadioButtons The dock in the UI
 * @param value The value pointers (to an int)
 * @return A binding between the two
 */
ChoiceBinding* ChoiceBinding::bindRadioButtons(int numRadioButtons, int value, ...) {
    // Create a button group from the variable argument list following initialValue
    QButtonGroup *buttonGroup = new QButtonGroup;
    va_list args;
    va_start(args, value);
    for (int id = 0; id < numRadioButtons; id++)
        buttonGroup->addButton(va_arg(args, QRadioButton *), id);
    va_end(args);

    // Bind the button group and the value together
    ChoiceBinding *binding = new ChoiceBinding(value);
    connect(buttonGroup, SIGNAL(buttonClicked(int)), binding, SLOT(intChanged(int)));

    // Set the initial value
    value = qMax(0, qMin(numRadioButtons - 1, value));
    buttonGroup->button(value)->click();

    return binding;
}

/**
 * @brief Binds tabs to values
 * @param tabs The tabs themselves
 * @param value The value pointer (to int)
 * @return A binding between the two
 */
ChoiceBinding* ChoiceBinding::bindTabs(QTabWidget *tabs, int &value) {
    // Bind the tabs and the value together
    ChoiceBinding *binding = new ChoiceBinding(value);
    connect(tabs, SIGNAL(currentChanged(int)), binding, SLOT(intChanged(int)));

    // Set the initial value
    value = qMax(0, qMin(tabs->count() - 1, value));
    tabs->setCurrentIndex(value);

    return binding;
}

/**
 * @brief Method to be called when an int changed
 * @param newValue the new int
 */
void ChoiceBinding::intChanged(int newValue) {
    m_value = newValue;
    emit dataChanged();
}
