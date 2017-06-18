#include "SettingsProviderMock.hpp"

QMap<QString, QVariant> SettingsProviderMock::values;

Mock<ISettingsProvider> SettingsProviderMock::get() {
    Mock<ISettingsProvider> mock;

    When(Method(mock, clear)).AlwaysReturn();

    When(Method(mock, getValue)).AlwaysDo([](const QString& key, const QVariant& defaultValue) -> QVariant {
        if (values.contains(key))
            return values[key];
        return defaultValue;
    });
    When(Method(mock, setValue)).AlwaysDo([](const QString& key, const QVariant& value){
        values[key] = value;
    });

    return mock;
}