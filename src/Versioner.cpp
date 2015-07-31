/*
 * Versioner.cpp
 *
 *  Created on: Jul 30, 2015
 *      Author: lingnan
 */

#include <bb/ApplicationInfo>
#include <Versioner.h>

Versioner::Versioner()
{
    qDebug() << "Reading settings...";
    _last = Version(_settings.value("last_version").toString());
    _current = bb::ApplicationInfo().version();
    _settings.setValue("last_version", _current.string());
}
