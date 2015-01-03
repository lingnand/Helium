/*
 * Buffer.cpp
 *
 *  Created on: Nov 23, 2014
 *      Author: lingnan
 */

#include <src/Buffer.h>
#include <stdio.h>
#include <sstream>

/*
 * Buffer contains a worker thread that continuously to
 * parse the input and render the html input for it
 *
 */

Buffer::Buffer():m_pSourceHighlight("xhtml.outlang")
{
    // Observation:
    // 1. style works, but font:color not
    // 2. html comment doesn't work, because it disappears when you ask for the
    //    text again from the text area
    //      so for the time being you are better off just using span to decorate the lines
    // 3. when you query for the text from the text area it won't be the exact
    //    same text you passed to it - specifically, unneeded whitespace are removed
    //      this means you probably shouldn't only search back for \n for a line:
    //      edge case -> the first line won't work (will include the pre tag)
    // 4. for the cursor position, html tags don't count, but newlines do count
    // 5. when you remove entire lines, their ids DO disappear from the output
    //    but you don't need to worry if any tag becomes broken up
//    m_pParsedContent = dummyBufferContent;
//    m_pParsedContent =
//    "<pre style=' color:#000000; '>"
//    "<tt name='1'><span style=' color:#008200; '>#include &lt;curl/curl.h&gt;</span></tt>\n"
//    "<tt name='2'><span style=' color:#008200; '>#define DEFAULT_N_DOWNLOADERS 1</span></tt>\n"
//    "<tt name='3'></tt>\n"
//    "<tt name='4'><span style=' color:#000000; font-weight:bold; '>typedef</span> <span style=' color:#0057ae; '>struct</span> <span style=' color:#000000; '>{</span></tt>\n"
//    "<tt name='5'>    <span style=' color:#0057ae; '>char</span> data<span style=' color:#000000; '>[</span><span style=' color:#b07e00; '>8192</span><span style=' color:#000000; '>];</span></tt>\n"
//    "<tt name='6'>    <span style=' color:#0057ae; '>size_t</span> length<span style=' color:#000000; '>;</span></tt>\n"
//    "<tt name='7'><span style=' color:#000000; '>}</span> mbuffer_t<span style=' color:#000000; '>;</span></tt>\n"
//    "<tt name='8'></tt>\n"
//    "</pre>";
    std::istringstream input;
    std::ostringstream output;
    std::string content = "private int someInteger;\n"
                          "private float someFloat;\n"
                          "public Object object('10')\n";
    input.str(content);
    m_pSourceHighlight.highlight(input, output, "cpp.lang");

    m_pParsedContent = QString::fromStdString(output.str());
}

const QString& Buffer::name() const
{
    return m_pName;
}

void Buffer::setName(const QString& name)
{
    if (name != m_pName) {
        m_pName = name;
        emit nameChanged(name);
    }
}

const QUrl& Buffer::imageSource() const
{
    return m_pImageSource;
}

const QString& Buffer::parsedContent() const
{
    return m_pParsedContent;
}

void Buffer::setContent(const QString& content)
{
    QString parsed = parseContent(content);
    m_pParsedContent = parsed;
    if (parsed != content) {
//        fprintf(stdout, "content changed to: %s\n", content.toLocal8Bit().data());
        emit parsedContentChanged(parsed);
    }
}

const QString& Buffer::parseContent(const QString& content)
{
    return content;
}

