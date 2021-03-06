/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Qt/qdeclarativedebug.h>
#include <bb/cascades/ProgressIndicatorState>
#include <libqgit2/qgitglobal.h>
#include <libqgit2/qgitrepository.h>
#include <libqgit2/qgitremote.h>
#include <Helium.h>
#include <HighlightType.h>
#include <BufferState.h>
#include <Replacement.h>
#include <Progress.h>
#include <StateChangeContext.h>
#include <HtmlBufferChangeParser.h>

Q_DECL_EXPORT int main(int argc, char **argv)
{
    qRegisterMetaType<bb::cascades::ProgressIndicatorState::Type>(
            "bb::cascades::ProgressIndicatorState::Type");
    qRegisterMetaType<HighlightType>("HighlightType");
    qRegisterMetaType<BufferState>("BufferState&");
    qRegisterMetaType<BufferState>("BufferState");
    qRegisterMetaType<BufferStateChange>("BufferStateChange");
    qRegisterMetaType<StateChangeContext>("StateChangeContext&");
    qRegisterMetaType<Progress>("Progress");
    qRegisterMetaType<QList<Replacement> >("QList<Replacement>");
    qRegisterMetaType<QList<QString> >("QList<QString>");
    qRegisterMetaType<ParserPosition>("ParserPosition");
    qRegisterMetaType<LibQGit2::StatusList>("LibQGit2::StatusList");
    qRegisterMetaType<LibQGit2::Repository::ResetType>("LibQGit2::Repository::ResetType");
    qRegisterMetaType<LibQGit2::Object>("LibQGit2::Object");
    qRegisterMetaType<LibQGit2::Credentials>("LibQGit2::Credentials");
    qRegisterMetaType<LibQGit2::Reference>();
    qRegisterMetaType<LibQGit2::Remote *>();

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    // init LibQGit2
    LibQGit2::initLibQGit2();
    Helium app(argc, argv);
    int ret = Helium::exec();
    LibQGit2::shutdownLibQGit2();
    return ret;
}
