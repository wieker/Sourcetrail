#include "SourceGroupCxxSonargraph.h"

#include "Application.h"
#include "ApplicationSettings.h"
#include "CxxIndexerCommandProvider.h"
#include "IndexerCommandCxx.h"
#include "MessageStatus.h"
#include "SourceGroupSettingsCxxSonargraph.h"
#include "SonargraphProject.h"
#include "utility.h"

SourceGroupCxxSonargraph::SourceGroupCxxSonargraph(std::shared_ptr<SourceGroupSettingsCxxSonargraph> settings)
	: m_settings(settings)
{
}

bool SourceGroupCxxSonargraph::prepareIndexing()
{
	FilePath sonargraphProjectPath = m_settings->getSonargraphProjectPathExpandedAndAbsolute();
	if (!sonargraphProjectPath.empty() && !sonargraphProjectPath.exists())
	{
		std::wstring error = L"Can't refresh project. The referenced Sonargraph project does not exist anymore: "
			+ sonargraphProjectPath.wstr();
		MessageStatus(error, true).dispatch();
		Application::getInstance()->handleDialog(error, { L"Ok" });
	}
	return true;
}

std::set<FilePath> SourceGroupCxxSonargraph::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
{
	return SourceGroup::filterToContainedFilePaths(
		filePaths,
		getAllSourceFilePaths(),
		utility::toSet(m_settings->getIndexedHeaderPathsExpandedAndAbsolute()),
		std::vector<FilePathFilter>()
	);
}

std::set<FilePath> SourceGroupCxxSonargraph::getAllSourceFilePaths() const
{
	std::set<FilePath> sourceFilePaths;
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		for (const FilePath& filePath : project->getAllSourceFilePathsCanonical())
		{
			if (filePath.exists())
			{
				sourceFilePaths.insert(filePath);
			}
		}
	}
	return sourceFilePaths;
}

std::shared_ptr<IndexerCommandProvider> SourceGroupCxxSonargraph::getIndexerCommandProvider(const RefreshInfo& info) const
{
	std::shared_ptr<CxxIndexerCommandProvider> provider = std::make_shared<CxxIndexerCommandProvider>();
	if (std::shared_ptr<Sonargraph::Project> project = Sonargraph::Project::load(
		m_settings->getSonargraphProjectPathExpandedAndAbsolute(), getLanguage()
	))
	{
		for (std::shared_ptr<IndexerCommand> indexerCommand : project->getIndexerCommands(m_settings, ApplicationSettings::getInstance()))
		{
			if (std::shared_ptr<IndexerCommandCxx> indexerCommandCxx = std::dynamic_pointer_cast<IndexerCommandCxx>(indexerCommand))
			{
				if (info.filesToIndex.find(indexerCommand->getSourceFilePath()) != info.filesToIndex.end())
				{
					provider->addCommand(indexerCommandCxx);
				}
			}
		}
	}
	return provider;
}

std::vector<std::shared_ptr<IndexerCommand>> SourceGroupCxxSonargraph::getIndexerCommands(const RefreshInfo& info) const
{
	return getIndexerCommandProvider(info)->consumeAllCommands();
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxxSonargraph::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupCxxSonargraph::getSourceGroupSettings() const
{
	return m_settings;
}
