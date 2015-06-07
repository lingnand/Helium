# Config.pri file version 2.0. Auto-generated by IDE. Any changes made by user will be lost!
BASEDIR = $$quote($$_PRO_FILE_PWD_)

device {
    CONFIG(debug, debug|release) {
        profile {
            INCLUDEPATH += $$quote(/opt/bbndk/lib_10_3_0_698/boost/include) \
                $$quote(${QNX_TARGET}/usr/include/bb/system)

            DEPENDPATH += $$quote(/opt/bbndk/lib_10_3_0_698/boost/include) \
                $$quote(${QNX_TARGET}/usr/include/bb/system)

            LIBS += -lboost_regex \
                -lbbcascadespickers \
                -lbbsystem

            LIBS += $$quote(-L/opt/bbndk/lib_10_3_0_698/boost/lib)

            PRE_TARGETDEPS += $$quote(/opt/bbndk/lib_10_3_0_698/boost/lib)

            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        } else {
            INCLUDEPATH += $$quote(/opt/bbndk/lib_10_3_0_698/boost/include) \
                $$quote(${QNX_TARGET}/usr/include/bb/system)

            DEPENDPATH += $$quote(/opt/bbndk/lib_10_3_0_698/boost/include) \
                $$quote(${QNX_TARGET}/usr/include/bb/system)

            LIBS += -lboost_regex \
                -lbbcascadespickers \
                -lbbsystem

            LIBS += $$quote(-L/opt/bbndk/lib_10_3_0_698/boost/lib)

            PRE_TARGETDEPS += $$quote(/opt/bbndk/lib_10_3_0_698/boost/lib)

            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }

    }

    CONFIG(release, debug|release) {
        !profile {
            INCLUDEPATH += $$quote(/opt/bbndk/lib_10_3_0_698/boost/include) \
                $$quote(${QNX_TARGET}/usr/include/bb/system)

            DEPENDPATH += $$quote(/opt/bbndk/lib_10_3_0_698/boost/include) \
                $$quote(${QNX_TARGET}/usr/include/bb/system)

            LIBS += -lboost_regex \
                -lbbcascadespickers \
                -lbbsystem

            LIBS += $$quote(-L/opt/bbndk/lib_10_3_0_698/boost/lib)

            PRE_TARGETDEPS += $$quote(/opt/bbndk/lib_10_3_0_698/boost/lib)

            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

simulator {
    CONFIG(debug, debug|release) {
        !profile {
            INCLUDEPATH += $$quote(${QNX_TARGET}/usr/include/bb/system)

            DEPENDPATH += $$quote(${QNX_TARGET}/usr/include/bb/system)

            LIBS += -lbbcascadespickers \
                -lbbsystem

            CONFIG += \
                config_pri_assets \
                config_pri_source_group1
        }
    }
}

config_pri_assets {
    OTHER_FILES += \
        $$quote($$BASEDIR/assets/images/filetype/_blank.png) \
        $$quote($$BASEDIR/assets/images/filetype/ada.png) \
        $$quote($$BASEDIR/assets/images/filetype/applescript.png) \
        $$quote($$BASEDIR/assets/images/filetype/asm.png) \
        $$quote($$BASEDIR/assets/images/filetype/awk.png) \
        $$quote($$BASEDIR/assets/images/filetype/bat.png) \
        $$quote($$BASEDIR/assets/images/filetype/bib.png) \
        $$quote($$BASEDIR/assets/images/filetype/bison.png) \
        $$quote($$BASEDIR/assets/images/filetype/c.png) \
        $$quote($$BASEDIR/assets/images/filetype/caml.png) \
        $$quote($$BASEDIR/assets/images/filetype/changelog.png) \
        $$quote($$BASEDIR/assets/images/filetype/clipper.png) \
        $$quote($$BASEDIR/assets/images/filetype/cobol.png) \
        $$quote($$BASEDIR/assets/images/filetype/coffeescript.png) \
        $$quote($$BASEDIR/assets/images/filetype/conf.png) \
        $$quote($$BASEDIR/assets/images/filetype/cpp.png) \
        $$quote($$BASEDIR/assets/images/filetype/csharp.png) \
        $$quote($$BASEDIR/assets/images/filetype/css.png) \
        $$quote($$BASEDIR/assets/images/filetype/d.png) \
        $$quote($$BASEDIR/assets/images/filetype/desktop.png) \
        $$quote($$BASEDIR/assets/images/filetype/diff.png) \
        $$quote($$BASEDIR/assets/images/filetype/erlang.png) \
        $$quote($$BASEDIR/assets/images/filetype/errors.png) \
        $$quote($$BASEDIR/assets/images/filetype/fixed-fortran.png) \
        $$quote($$BASEDIR/assets/images/filetype/flex.png) \
        $$quote($$BASEDIR/assets/images/filetype/fortran.png) \
        $$quote($$BASEDIR/assets/images/filetype/glsl.png) \
        $$quote($$BASEDIR/assets/images/filetype/haskell.png) \
        $$quote($$BASEDIR/assets/images/filetype/haskell_literate.png) \
        $$quote($$BASEDIR/assets/images/filetype/haxe.png) \
        $$quote($$BASEDIR/assets/images/filetype/html.png) \
        $$quote($$BASEDIR/assets/images/filetype/islisp.png) \
        $$quote($$BASEDIR/assets/images/filetype/java.png) \
        $$quote($$BASEDIR/assets/images/filetype/javalog.png) \
        $$quote($$BASEDIR/assets/images/filetype/javascript.png) \
        $$quote($$BASEDIR/assets/images/filetype/langdef.png) \
        $$quote($$BASEDIR/assets/images/filetype/latex.png) \
        $$quote($$BASEDIR/assets/images/filetype/ldap.png) \
        $$quote($$BASEDIR/assets/images/filetype/lilypond.png) \
        $$quote($$BASEDIR/assets/images/filetype/lisp.png) \
        $$quote($$BASEDIR/assets/images/filetype/log.png) \
        $$quote($$BASEDIR/assets/images/filetype/logtalk.png) \
        $$quote($$BASEDIR/assets/images/filetype/lsm.png) \
        $$quote($$BASEDIR/assets/images/filetype/lua.png) \
        $$quote($$BASEDIR/assets/images/filetype/m4.png) \
        $$quote($$BASEDIR/assets/images/filetype/makefile.png) \
        $$quote($$BASEDIR/assets/images/filetype/manifest.png) \
        $$quote($$BASEDIR/assets/images/filetype/opa.png) \
        $$quote($$BASEDIR/assets/images/filetype/outlang.png) \
        $$quote($$BASEDIR/assets/images/filetype/oz.png) \
        $$quote($$BASEDIR/assets/images/filetype/pascal.png) \
        $$quote($$BASEDIR/assets/images/filetype/pc.png) \
        $$quote($$BASEDIR/assets/images/filetype/perl.png) \
        $$quote($$BASEDIR/assets/images/filetype/php.png) \
        $$quote($$BASEDIR/assets/images/filetype/po.png) \
        $$quote($$BASEDIR/assets/images/filetype/postscript.png) \
        $$quote($$BASEDIR/assets/images/filetype/prolog.png) \
        $$quote($$BASEDIR/assets/images/filetype/properties.png) \
        $$quote($$BASEDIR/assets/images/filetype/proto.png) \
        $$quote($$BASEDIR/assets/images/filetype/python.png) \
        $$quote($$BASEDIR/assets/images/filetype/ruby.png) \
        $$quote($$BASEDIR/assets/images/filetype/scala.png) \
        $$quote($$BASEDIR/assets/images/filetype/scheme.png) \
        $$quote($$BASEDIR/assets/images/filetype/sh.png) \
        $$quote($$BASEDIR/assets/images/filetype/slang.png) \
        $$quote($$BASEDIR/assets/images/filetype/sml.png) \
        $$quote($$BASEDIR/assets/images/filetype/spec.png) \
        $$quote($$BASEDIR/assets/images/filetype/sql.png) \
        $$quote($$BASEDIR/assets/images/filetype/style.png) \
        $$quote($$BASEDIR/assets/images/filetype/tcl.png) \
        $$quote($$BASEDIR/assets/images/filetype/texinfo.png) \
        $$quote($$BASEDIR/assets/images/filetype/tml.png) \
        $$quote($$BASEDIR/assets/images/filetype/upc.png) \
        $$quote($$BASEDIR/assets/images/filetype/vala.png) \
        $$quote($$BASEDIR/assets/images/filetype/vbscript.png) \
        $$quote($$BASEDIR/assets/images/filetype/xml.png) \
        $$quote($$BASEDIR/assets/images/filetype/xorg.png) \
        $$quote($$BASEDIR/assets/images/ic_add.png) \
        $$quote($$BASEDIR/assets/images/ic_backward.png) \
        $$quote($$BASEDIR/assets/images/ic_cancel.png) \
        $$quote($$BASEDIR/assets/images/ic_clear.png) \
        $$quote($$BASEDIR/assets/images/ic_copy_link.png) \
        $$quote($$BASEDIR/assets/images/ic_edit.png) \
        $$quote($$BASEDIR/assets/images/ic_forward.png) \
        $$quote($$BASEDIR/assets/images/ic_next.png) \
        $$quote($$BASEDIR/assets/images/ic_open.png) \
        $$quote($$BASEDIR/assets/images/ic_play.png) \
        $$quote($$BASEDIR/assets/images/ic_previous.png) \
        $$quote($$BASEDIR/assets/images/ic_properties.png) \
        $$quote($$BASEDIR/assets/images/ic_redo.png) \
        $$quote($$BASEDIR/assets/images/ic_reload.png) \
        $$quote($$BASEDIR/assets/images/ic_rename.png) \
        $$quote($$BASEDIR/assets/images/ic_save.png) \
        $$quote($$BASEDIR/assets/images/ic_save_as.png) \
        $$quote($$BASEDIR/assets/images/ic_search.png) \
        $$quote($$BASEDIR/assets/images/ic_undo.png) \
        $$quote($$BASEDIR/assets/images/ic_view_details.png) \
        $$quote($$BASEDIR/assets/srchilite/ada.lang) \
        $$quote($$BASEDIR/assets/srchilite/applescript.lang) \
        $$quote($$BASEDIR/assets/srchilite/asm.lang) \
        $$quote($$BASEDIR/assets/srchilite/awk.lang) \
        $$quote($$BASEDIR/assets/srchilite/bat.lang) \
        $$quote($$BASEDIR/assets/srchilite/bib.lang) \
        $$quote($$BASEDIR/assets/srchilite/bison.lang) \
        $$quote($$BASEDIR/assets/srchilite/c.lang) \
        $$quote($$BASEDIR/assets/srchilite/c_comment.lang) \
        $$quote($$BASEDIR/assets/srchilite/c_string.lang) \
        $$quote($$BASEDIR/assets/srchilite/caml.lang) \
        $$quote($$BASEDIR/assets/srchilite/changelog.lang) \
        $$quote($$BASEDIR/assets/srchilite/clike_vardeclaration.lang) \
        $$quote($$BASEDIR/assets/srchilite/clipper.lang) \
        $$quote($$BASEDIR/assets/srchilite/cobol.lang) \
        $$quote($$BASEDIR/assets/srchilite/coffeescript.lang) \
        $$quote($$BASEDIR/assets/srchilite/conf.lang) \
        $$quote($$BASEDIR/assets/srchilite/cpp.lang) \
        $$quote($$BASEDIR/assets/srchilite/csharp.lang) \
        $$quote($$BASEDIR/assets/srchilite/css.lang) \
        $$quote($$BASEDIR/assets/srchilite/d.lang) \
        $$quote($$BASEDIR/assets/srchilite/default.lang) \
        $$quote($$BASEDIR/assets/srchilite/default.style) \
        $$quote($$BASEDIR/assets/srchilite/desktop.lang) \
        $$quote($$BASEDIR/assets/srchilite/diff.lang) \
        $$quote($$BASEDIR/assets/srchilite/erlang.lang) \
        $$quote($$BASEDIR/assets/srchilite/errors.lang) \
        $$quote($$BASEDIR/assets/srchilite/extreme_comment.lang) \
        $$quote($$BASEDIR/assets/srchilite/extreme_comment2.lang) \
        $$quote($$BASEDIR/assets/srchilite/extreme_comment3.lang) \
        $$quote($$BASEDIR/assets/srchilite/extreme_comment4.lang) \
        $$quote($$BASEDIR/assets/srchilite/fixed-fortran.lang) \
        $$quote($$BASEDIR/assets/srchilite/flex.lang) \
        $$quote($$BASEDIR/assets/srchilite/fortran.lang) \
        $$quote($$BASEDIR/assets/srchilite/function.lang) \
        $$quote($$BASEDIR/assets/srchilite/glsl.lang) \
        $$quote($$BASEDIR/assets/srchilite/haskell.lang) \
        $$quote($$BASEDIR/assets/srchilite/haskell_literate.lang) \
        $$quote($$BASEDIR/assets/srchilite/haxe.lang) \
        $$quote($$BASEDIR/assets/srchilite/html.lang) \
        $$quote($$BASEDIR/assets/srchilite/html_common.outlang) \
        $$quote($$BASEDIR/assets/srchilite/html_ref.outlang) \
        $$quote($$BASEDIR/assets/srchilite/html_simple.lang) \
        $$quote($$BASEDIR/assets/srchilite/islisp.lang) \
        $$quote($$BASEDIR/assets/srchilite/java.lang) \
        $$quote($$BASEDIR/assets/srchilite/javalog.lang) \
        $$quote($$BASEDIR/assets/srchilite/javascript.lang) \
        $$quote($$BASEDIR/assets/srchilite/key_string.lang) \
        $$quote($$BASEDIR/assets/srchilite/lang.map) \
        $$quote($$BASEDIR/assets/srchilite/langdef.lang) \
        $$quote($$BASEDIR/assets/srchilite/latex.lang) \
        $$quote($$BASEDIR/assets/srchilite/ldap.lang) \
        $$quote($$BASEDIR/assets/srchilite/lilypond.lang) \
        $$quote($$BASEDIR/assets/srchilite/lisp.lang) \
        $$quote($$BASEDIR/assets/srchilite/log.lang) \
        $$quote($$BASEDIR/assets/srchilite/logtalk.lang) \
        $$quote($$BASEDIR/assets/srchilite/lsm.lang) \
        $$quote($$BASEDIR/assets/srchilite/lua.lang) \
        $$quote($$BASEDIR/assets/srchilite/m4.lang) \
        $$quote($$BASEDIR/assets/srchilite/makefile.lang) \
        $$quote($$BASEDIR/assets/srchilite/manifest.lang) \
        $$quote($$BASEDIR/assets/srchilite/nohilite.lang) \
        $$quote($$BASEDIR/assets/srchilite/number.lang) \
        $$quote($$BASEDIR/assets/srchilite/opa.lang) \
        $$quote($$BASEDIR/assets/srchilite/outlang.lang) \
        $$quote($$BASEDIR/assets/srchilite/oz.lang) \
        $$quote($$BASEDIR/assets/srchilite/pascal.lang) \
        $$quote($$BASEDIR/assets/srchilite/pc.lang) \
        $$quote($$BASEDIR/assets/srchilite/perl.lang) \
        $$quote($$BASEDIR/assets/srchilite/php.lang) \
        $$quote($$BASEDIR/assets/srchilite/po.lang) \
        $$quote($$BASEDIR/assets/srchilite/postscript.lang) \
        $$quote($$BASEDIR/assets/srchilite/prolog.lang) \
        $$quote($$BASEDIR/assets/srchilite/properties.lang) \
        $$quote($$BASEDIR/assets/srchilite/proto.lang) \
        $$quote($$BASEDIR/assets/srchilite/python.lang) \
        $$quote($$BASEDIR/assets/srchilite/ruby.lang) \
        $$quote($$BASEDIR/assets/srchilite/scala.lang) \
        $$quote($$BASEDIR/assets/srchilite/scheme.lang) \
        $$quote($$BASEDIR/assets/srchilite/script_comment.lang) \
        $$quote($$BASEDIR/assets/srchilite/sh.lang) \
        $$quote($$BASEDIR/assets/srchilite/slang.lang) \
        $$quote($$BASEDIR/assets/srchilite/sml.lang) \
        $$quote($$BASEDIR/assets/srchilite/spec.lang) \
        $$quote($$BASEDIR/assets/srchilite/sql.lang) \
        $$quote($$BASEDIR/assets/srchilite/style.lang) \
        $$quote($$BASEDIR/assets/srchilite/symbols.lang) \
        $$quote($$BASEDIR/assets/srchilite/tcl.lang) \
        $$quote($$BASEDIR/assets/srchilite/texinfo.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_formatting.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_formatting_all.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_glue.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macrolinks.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macros.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macros1.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macros2.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macrosdelayed1.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macrosdelayed2.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macrosdelayed3.lang) \
        $$quote($$BASEDIR/assets/srchilite/tml_macrotokens.lang) \
        $$quote($$BASEDIR/assets/srchilite/todo.lang) \
        $$quote($$BASEDIR/assets/srchilite/upc.lang) \
        $$quote($$BASEDIR/assets/srchilite/url.lang) \
        $$quote($$BASEDIR/assets/srchilite/vala.lang) \
        $$quote($$BASEDIR/assets/srchilite/vbscript.lang) \
        $$quote($$BASEDIR/assets/srchilite/xhtml.outlang) \
        $$quote($$BASEDIR/assets/srchilite/xhtml_common.outlang) \
        $$quote($$BASEDIR/assets/srchilite/xml.lang) \
        $$quote($$BASEDIR/assets/srchilite/xorg.lang)
}

config_pri_source_group1 {
    SOURCES += \
        $$quote($$BASEDIR/src/AppearanceSettings.cpp) \
        $$quote($$BASEDIR/src/AppearanceSettingsPage.cpp) \
        $$quote($$BASEDIR/src/AppearanceSettingsStorage.cpp) \
        $$quote($$BASEDIR/src/Buffer.cpp) \
        $$quote($$BASEDIR/src/BufferHistory.cpp) \
        $$quote($$BASEDIR/src/BufferState.cpp) \
        $$quote($$BASEDIR/src/BufferWorker.cpp) \
        $$quote($$BASEDIR/src/CmdRunProfile.cpp) \
        $$quote($$BASEDIR/src/CmdRunProfileManager.cpp) \
        $$quote($$BASEDIR/src/CmdRunProfileSettingsUI.cpp) \
        $$quote($$BASEDIR/src/Defaults.cpp) \
        $$quote($$BASEDIR/src/FilePropertiesPage.cpp) \
        $$quote($$BASEDIR/src/Filetype.cpp) \
        $$quote($$BASEDIR/src/FiletypeMap.cpp) \
        $$quote($$BASEDIR/src/FiletypeMapSettingsPage.cpp) \
        $$quote($$BASEDIR/src/FiletypeMapStorage.cpp) \
        $$quote($$BASEDIR/src/FiletypeSettingsUI.cpp) \
        $$quote($$BASEDIR/src/FindMode.cpp) \
        $$quote($$BASEDIR/src/GeneralSettings.cpp) \
        $$quote($$BASEDIR/src/GeneralSettingsPage.cpp) \
        $$quote($$BASEDIR/src/GeneralSettingsStorage.cpp) \
        $$quote($$BASEDIR/src/Helium.cpp) \
        $$quote($$BASEDIR/src/HelpPage.cpp) \
        $$quote($$BASEDIR/src/HighlightRangePicker.cpp) \
        $$quote($$BASEDIR/src/HighlightType.cpp) \
        $$quote($$BASEDIR/src/HtmlBufferChangeParser.cpp) \
        $$quote($$BASEDIR/src/HtmlParser.cpp) \
        $$quote($$BASEDIR/src/ModKeyListener.cpp) \
        $$quote($$BASEDIR/src/MultiViewPane.cpp) \
        $$quote($$BASEDIR/src/NormalMode.cpp) \
        $$quote($$BASEDIR/src/Replacement.cpp) \
        $$quote($$BASEDIR/src/RepushablePage.cpp) \
        $$quote($$BASEDIR/src/RunProfile.cpp) \
        $$quote($$BASEDIR/src/RunProfileManager.cpp) \
        $$quote($$BASEDIR/src/RunProfileSettingsUI.cpp) \
        $$quote($$BASEDIR/src/Segment.cpp) \
        $$quote($$BASEDIR/src/SettingsPage.cpp) \
        $$quote($$BASEDIR/src/Utility.cpp) \
        $$quote($$BASEDIR/src/View.cpp) \
        $$quote($$BASEDIR/src/ViewMode.cpp) \
        $$quote($$BASEDIR/src/WebRunProfile.cpp) \
        $$quote($$BASEDIR/src/WebRunProfileManager.cpp) \
        $$quote($$BASEDIR/src/WebRunProfileSettingsUI.cpp) \
        $$quote($$BASEDIR/src/main.cpp) \
        $$quote($$BASEDIR/src/srchilite/bufferedoutput.cpp) \
        $$quote($$BASEDIR/src/srchilite/chartranslator.cc) \
        $$quote($$BASEDIR/src/srchilite/debuglistener.cpp) \
        $$quote($$BASEDIR/src/srchilite/delimitedlangelem.cpp) \
        $$quote($$BASEDIR/src/srchilite/docgenerator.cc) \
        $$quote($$BASEDIR/src/srchilite/doctemplate.cpp) \
        $$quote($$BASEDIR/src/srchilite/fileinfo.cpp) \
        $$quote($$BASEDIR/src/srchilite/fileutil.cc) \
        $$quote($$BASEDIR/src/srchilite/formatter.cpp) \
        $$quote($$BASEDIR/src/srchilite/formattermanager.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlightbuilderexception.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlightrule.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlightrulefactory.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlightstate.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlightstatebuilder.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlightstatebuilder_dbtab.cc) \
        $$quote($$BASEDIR/src/srchilite/highlightstateprinter.cpp) \
        $$quote($$BASEDIR/src/srchilite/highlighttoken.cpp) \
        $$quote($$BASEDIR/src/srchilite/instances.cpp) \
        $$quote($$BASEDIR/src/srchilite/ioexception.cpp) \
        $$quote($$BASEDIR/src/srchilite/langdefmanager.cpp) \
        $$quote($$BASEDIR/src/srchilite/langdefparser.cc) \
        $$quote($$BASEDIR/src/srchilite/langdefscanner.cc) \
        $$quote($$BASEDIR/src/srchilite/langelem.cpp) \
        $$quote($$BASEDIR/src/srchilite/langelems.cpp) \
        $$quote($$BASEDIR/src/srchilite/langelemsprinter.cpp) \
        $$quote($$BASEDIR/src/srchilite/langelemsprinter_dbtab.cc) \
        $$quote($$BASEDIR/src/srchilite/langmap.cpp) \
        $$quote($$BASEDIR/src/srchilite/languageinfer.cpp) \
        $$quote($$BASEDIR/src/srchilite/linenumgenerator.cpp) \
        $$quote($$BASEDIR/src/srchilite/lineranges.cpp) \
        $$quote($$BASEDIR/src/srchilite/namedsubexpslangelem.cpp) \
        $$quote($$BASEDIR/src/srchilite/outlangdefparser.cc) \
        $$quote($$BASEDIR/src/srchilite/outlangdefscanner.cc) \
        $$quote($$BASEDIR/src/srchilite/parserexception.cpp) \
        $$quote($$BASEDIR/src/srchilite/preformatter.cpp) \
        $$quote($$BASEDIR/src/srchilite/regexhighlightrule.cpp) \
        $$quote($$BASEDIR/src/srchilite/regexpreprocessor.cpp) \
        $$quote($$BASEDIR/src/srchilite/regexranges.cpp) \
        $$quote($$BASEDIR/src/srchilite/regexrulefactory.cpp) \
        $$quote($$BASEDIR/src/srchilite/settings.cpp) \
        $$quote($$BASEDIR/src/srchilite/sourcefilehighlighter.cpp) \
        $$quote($$BASEDIR/src/srchilite/sourcehighlight.cpp) \
        $$quote($$BASEDIR/src/srchilite/sourcehighlighter.cpp) \
        $$quote($$BASEDIR/src/srchilite/sourcehighlightutils.cpp) \
        $$quote($$BASEDIR/src/srchilite/srcuntabifier.cpp) \
        $$quote($$BASEDIR/src/srchilite/statelangelem.cpp) \
        $$quote($$BASEDIR/src/srchilite/statestartlangelem.cpp) \
        $$quote($$BASEDIR/src/srchilite/stopwatch.cpp) \
        $$quote($$BASEDIR/src/srchilite/stringdef.cpp) \
        $$quote($$BASEDIR/src/srchilite/stringlistlangelem.cpp) \
        $$quote($$BASEDIR/src/srchilite/stringtable.cpp) \
        $$quote($$BASEDIR/src/srchilite/stylecssparser.cc) \
        $$quote($$BASEDIR/src/srchilite/stylecssscanner.cc) \
        $$quote($$BASEDIR/src/srchilite/stylefileparser.cpp) \
        $$quote($$BASEDIR/src/srchilite/styleparser.cc) \
        $$quote($$BASEDIR/src/srchilite/stylescanner.cc) \
        $$quote($$BASEDIR/src/srchilite/substfun.cpp) \
        $$quote($$BASEDIR/src/srchilite/textstyle.cpp) \
        $$quote($$BASEDIR/src/srchilite/textstylebuilder.cpp) \
        $$quote($$BASEDIR/src/srchilite/textstyleformatter.cpp) \
        $$quote($$BASEDIR/src/srchilite/textstyleformatterfactory.cpp) \
        $$quote($$BASEDIR/src/srchilite/utils.cpp) \
        $$quote($$BASEDIR/src/srchilite/vardefinitions.cpp) \
        $$quote($$BASEDIR/src/srchilite/verbosity.cpp) \
        $$quote($$BASEDIR/src/srchilite/wordtokenizer.cpp)

    HEADERS += \
        $$quote($$BASEDIR/src/AppearanceSettings.h) \
        $$quote($$BASEDIR/src/AppearanceSettingsPage.h) \
        $$quote($$BASEDIR/src/AppearanceSettingsStorage.h) \
        $$quote($$BASEDIR/src/Buffer.h) \
        $$quote($$BASEDIR/src/BufferHistory.h) \
        $$quote($$BASEDIR/src/BufferState.h) \
        $$quote($$BASEDIR/src/BufferWorker.h) \
        $$quote($$BASEDIR/src/CmdRunProfile.h) \
        $$quote($$BASEDIR/src/CmdRunProfileManager.h) \
        $$quote($$BASEDIR/src/CmdRunProfileSettingsUI.h) \
        $$quote($$BASEDIR/src/Defaults.h) \
        $$quote($$BASEDIR/src/FilePropertiesPage.h) \
        $$quote($$BASEDIR/src/Filetype.h) \
        $$quote($$BASEDIR/src/FiletypeMap.h) \
        $$quote($$BASEDIR/src/FiletypeMapSettingsPage.h) \
        $$quote($$BASEDIR/src/FiletypeMapStorage.h) \
        $$quote($$BASEDIR/src/FiletypeSettingsUI.h) \
        $$quote($$BASEDIR/src/FindMode.h) \
        $$quote($$BASEDIR/src/GeneralSettings.h) \
        $$quote($$BASEDIR/src/GeneralSettingsPage.h) \
        $$quote($$BASEDIR/src/GeneralSettingsStorage.h) \
        $$quote($$BASEDIR/src/Helium.h) \
        $$quote($$BASEDIR/src/HelpPage.h) \
        $$quote($$BASEDIR/src/HighlightRangePicker.h) \
        $$quote($$BASEDIR/src/HighlightStateData.h) \
        $$quote($$BASEDIR/src/HighlightType.h) \
        $$quote($$BASEDIR/src/HtmlBufferChangeParser.h) \
        $$quote($$BASEDIR/src/HtmlParser.h) \
        $$quote($$BASEDIR/src/ModKeyListener.h) \
        $$quote($$BASEDIR/src/MultiViewPane.h) \
        $$quote($$BASEDIR/src/NormalMode.h) \
        $$quote($$BASEDIR/src/Replacement.h) \
        $$quote($$BASEDIR/src/RepushablePage.h) \
        $$quote($$BASEDIR/src/RunProfile.h) \
        $$quote($$BASEDIR/src/RunProfileManager.h) \
        $$quote($$BASEDIR/src/RunProfileSettingsUI.h) \
        $$quote($$BASEDIR/src/Segment.h) \
        $$quote($$BASEDIR/src/SettingsPage.h) \
        $$quote($$BASEDIR/src/SignalBlocker.h) \
        $$quote($$BASEDIR/src/StateChangeContext.h) \
        $$quote($$BASEDIR/src/Utility.h) \
        $$quote($$BASEDIR/src/View.h) \
        $$quote($$BASEDIR/src/ViewMode.h) \
        $$quote($$BASEDIR/src/WebRunProfile.h) \
        $$quote($$BASEDIR/src/WebRunProfileManager.h) \
        $$quote($$BASEDIR/src/WebRunProfileSettingsUI.h) \
        $$quote($$BASEDIR/src/srchilite/bufferedoutput.h) \
        $$quote($$BASEDIR/src/srchilite/chartranslator.h) \
        $$quote($$BASEDIR/src/srchilite/colormap.h) \
        $$quote($$BASEDIR/src/srchilite/colors.h) \
        $$quote($$BASEDIR/src/srchilite/debuglistener.h) \
        $$quote($$BASEDIR/src/srchilite/delimitedlangelem.h) \
        $$quote($$BASEDIR/src/srchilite/docgenerator.h) \
        $$quote($$BASEDIR/src/srchilite/doctemplate.h) \
        $$quote($$BASEDIR/src/srchilite/eventgenerator.h) \
        $$quote($$BASEDIR/src/srchilite/fileinfo.h) \
        $$quote($$BASEDIR/src/srchilite/fileutil.h) \
        $$quote($$BASEDIR/src/srchilite/formatter.h) \
        $$quote($$BASEDIR/src/srchilite/formatterfactory.h) \
        $$quote($$BASEDIR/src/srchilite/formattermanager.h) \
        $$quote($$BASEDIR/src/srchilite/formatterparams.h) \
        $$quote($$BASEDIR/src/srchilite/highlightbuilderexception.h) \
        $$quote($$BASEDIR/src/srchilite/highlightevent.h) \
        $$quote($$BASEDIR/src/srchilite/highlighteventlistener.h) \
        $$quote($$BASEDIR/src/srchilite/highlightrule.h) \
        $$quote($$BASEDIR/src/srchilite/highlightrulefactory.h) \
        $$quote($$BASEDIR/src/srchilite/highlightstate.h) \
        $$quote($$BASEDIR/src/srchilite/highlightstatebuilder.h) \
        $$quote($$BASEDIR/src/srchilite/highlightstatebuilder.hpp) \
        $$quote($$BASEDIR/src/srchilite/highlightstateprinter.h) \
        $$quote($$BASEDIR/src/srchilite/highlighttoken.h) \
        $$quote($$BASEDIR/src/srchilite/instances.h) \
        $$quote($$BASEDIR/src/srchilite/ioexception.h) \
        $$quote($$BASEDIR/src/srchilite/keys.h) \
        $$quote($$BASEDIR/src/srchilite/langdefmanager.h) \
        $$quote($$BASEDIR/src/srchilite/langdefparser.h) \
        $$quote($$BASEDIR/src/srchilite/langdefparserfun.h) \
        $$quote($$BASEDIR/src/srchilite/langdefscanner.h) \
        $$quote($$BASEDIR/src/srchilite/langelem.h) \
        $$quote($$BASEDIR/src/srchilite/langelems.h) \
        $$quote($$BASEDIR/src/srchilite/langelemsprinter.h) \
        $$quote($$BASEDIR/src/srchilite/langelemsprinter.hpp) \
        $$quote($$BASEDIR/src/srchilite/langmap.h) \
        $$quote($$BASEDIR/src/srchilite/languageinfer.h) \
        $$quote($$BASEDIR/src/srchilite/linebuffer.h) \
        $$quote($$BASEDIR/src/srchilite/linenumgenerator.h) \
        $$quote($$BASEDIR/src/srchilite/lineranges.h) \
        $$quote($$BASEDIR/src/srchilite/matchingparameters.h) \
        $$quote($$BASEDIR/src/srchilite/namedsubexpslangelem.h) \
        $$quote($$BASEDIR/src/srchilite/outlangdefparser.h) \
        $$quote($$BASEDIR/src/srchilite/outlangdefparserfun.h) \
        $$quote($$BASEDIR/src/srchilite/outlangdefscanner.h) \
        $$quote($$BASEDIR/src/srchilite/parserexception.h) \
        $$quote($$BASEDIR/src/srchilite/parserinfo.h) \
        $$quote($$BASEDIR/src/srchilite/parsestruct.h) \
        $$quote($$BASEDIR/src/srchilite/parsestyles.h) \
        $$quote($$BASEDIR/src/srchilite/preformatter.h) \
        $$quote($$BASEDIR/src/srchilite/refposition.h) \
        $$quote($$BASEDIR/src/srchilite/regexhighlightrule.h) \
        $$quote($$BASEDIR/src/srchilite/regexpreprocessor.h) \
        $$quote($$BASEDIR/src/srchilite/regexranges.h) \
        $$quote($$BASEDIR/src/srchilite/regexrulefactory.h) \
        $$quote($$BASEDIR/src/srchilite/settings.h) \
        $$quote($$BASEDIR/src/srchilite/sourcefilehighlighter.h) \
        $$quote($$BASEDIR/src/srchilite/sourcehighlight.h) \
        $$quote($$BASEDIR/src/srchilite/sourcehighlighter.h) \
        $$quote($$BASEDIR/src/srchilite/sourcehighlightutils.h) \
        $$quote($$BASEDIR/src/srchilite/srcuntabifier.h) \
        $$quote($$BASEDIR/src/srchilite/statelangelem.h) \
        $$quote($$BASEDIR/src/srchilite/statestartlangelem.h) \
        $$quote($$BASEDIR/src/srchilite/stopwatch.h) \
        $$quote($$BASEDIR/src/srchilite/stringdef.h) \
        $$quote($$BASEDIR/src/srchilite/stringlistlangelem.h) \
        $$quote($$BASEDIR/src/srchilite/stringtable.h) \
        $$quote($$BASEDIR/src/srchilite/stylecssparser.h) \
        $$quote($$BASEDIR/src/srchilite/stylefileparser.h) \
        $$quote($$BASEDIR/src/srchilite/stylekey.h) \
        $$quote($$BASEDIR/src/srchilite/styleparser.h) \
        $$quote($$BASEDIR/src/srchilite/substfun.h) \
        $$quote($$BASEDIR/src/srchilite/textstyle.h) \
        $$quote($$BASEDIR/src/srchilite/textstylebuilder.h) \
        $$quote($$BASEDIR/src/srchilite/textstyleformatter.h) \
        $$quote($$BASEDIR/src/srchilite/textstyleformattercollection.h) \
        $$quote($$BASEDIR/src/srchilite/textstyleformatterfactory.h) \
        $$quote($$BASEDIR/src/srchilite/textstyles.h) \
        $$quote($$BASEDIR/src/srchilite/tostringcollection.h) \
        $$quote($$BASEDIR/src/srchilite/utils.h) \
        $$quote($$BASEDIR/src/srchilite/vardefinitions.h) \
        $$quote($$BASEDIR/src/srchilite/verbosity.h) \
        $$quote($$BASEDIR/src/srchilite/wordtokenizer.h)
}

INCLUDEPATH += $$quote($$BASEDIR/src/srchilite) \
    $$quote($$BASEDIR/src)

CONFIG += precompile_header

PRECOMPILED_HEADER = $$quote($$BASEDIR/precompiled.h)

lupdate_inclusion {
    SOURCES += \
        $$quote($$BASEDIR/../src/*.c) \
        $$quote($$BASEDIR/../src/*.c++) \
        $$quote($$BASEDIR/../src/*.cc) \
        $$quote($$BASEDIR/../src/*.cpp) \
        $$quote($$BASEDIR/../src/*.cxx) \
        $$quote($$BASEDIR/../src/srchilite/*.c) \
        $$quote($$BASEDIR/../src/srchilite/*.c++) \
        $$quote($$BASEDIR/../src/srchilite/*.cc) \
        $$quote($$BASEDIR/../src/srchilite/*.cpp) \
        $$quote($$BASEDIR/../src/srchilite/*.cxx) \
        $$quote($$BASEDIR/../assets/*.qml) \
        $$quote($$BASEDIR/../assets/*.js) \
        $$quote($$BASEDIR/../assets/*.qs) \
        $$quote($$BASEDIR/../assets/images/*.qml) \
        $$quote($$BASEDIR/../assets/images/*.js) \
        $$quote($$BASEDIR/../assets/images/*.qs) \
        $$quote($$BASEDIR/../assets/images/filetype/*.qml) \
        $$quote($$BASEDIR/../assets/images/filetype/*.js) \
        $$quote($$BASEDIR/../assets/images/filetype/*.qs) \
        $$quote($$BASEDIR/../assets/srchilite/*.qml) \
        $$quote($$BASEDIR/../assets/srchilite/*.js) \
        $$quote($$BASEDIR/../assets/srchilite/*.qs)

    HEADERS += \
        $$quote($$BASEDIR/../src/*.h) \
        $$quote($$BASEDIR/../src/*.h++) \
        $$quote($$BASEDIR/../src/*.hh) \
        $$quote($$BASEDIR/../src/*.hpp) \
        $$quote($$BASEDIR/../src/*.hxx)
}

TRANSLATIONS = $$quote($${TARGET}_zh_CN.ts) \
    $$quote($${TARGET}.ts)
