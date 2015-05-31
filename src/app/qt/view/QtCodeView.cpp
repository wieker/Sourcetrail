#include "qt/view/QtCodeView.h"

#include "utility/file/FileSystem.h"
#include "utility/text/TextAccess.h"

#include "qt/element/QtCodeFileList.h"
#include "qt/view/QtViewWidgetWrapper.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_showCodeSnippetsFunctor(std::bind(&QtCodeView::doShowCodeSnippets, this, std::placeholders::_1))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
	, m_doScrollToFirstActiveSnippetFunctor(std::bind(&QtCodeView::doScrollToFirstActiveSnippet, this))
	, m_focusTokenFunctor(std::bind(&QtCodeView::doFocusToken, this, std::placeholders::_1))
	, m_defocusTokenFunctor(std::bind(&QtCodeView::doDefocusToken, this))
{
	m_widget = new QtCodeFileList();
	setStyleSheet(m_widget);
}

QtCodeView::~QtCodeView()
{
}

void QtCodeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtCodeView::initView()
{
}

void QtCodeView::refreshView()
{
	m_refreshViewFunctor();
}

void QtCodeView::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
}

void QtCodeView::setErrorMessages(const std::vector<std::string>& errorMessages)
{
	m_errorMessages = errorMessages;
}

void QtCodeView::showCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	m_showCodeSnippetsFunctor(snippets);
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::scrollToFirstActiveSnippet()
{
	m_doScrollToFirstActiveSnippetFunctor();
}

void QtCodeView::focusToken(const Id tokenId)
{
	m_focusTokenFunctor(tokenId);
}

void QtCodeView::defocusToken()
{
	m_defocusTokenFunctor();
}

void QtCodeView::doRefreshView()
{
	setStyleSheet(m_widget);
}

void QtCodeView::doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	m_widget->clearCodeSnippets();

	m_widget->setActiveTokenIds(m_activeTokenIds);
	m_widget->setErrorMessages(m_errorMessages);

	for (const CodeSnippetParams& params : snippets)
	{
		m_widget->addCodeSnippet(params.startLineNumber, params.title, params.code, params.locationFile);
	}
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	m_widget->addCodeSnippet(1, params.title, params.code, params.locationFile);
}

void QtCodeView::doScrollToFirstActiveSnippet()
{
	m_widget->setActiveTokenIds(m_activeTokenIds);
	m_widget->scrollToFirstActiveSnippet();
}

void QtCodeView::doFocusToken(const Id tokenId)
{
	m_widget->focusToken(tokenId);
}

void QtCodeView::doDefocusToken()
{
	m_widget->defocusToken();
}

void QtCodeView::setStyleSheet(QWidget* widget) const
{
	widget->setStyleSheet(TextAccess::createFromFile("data/gui/code_view/code_view.css")->getText().c_str());
}
