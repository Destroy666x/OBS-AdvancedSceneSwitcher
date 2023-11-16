#pragma once
#include "macro-action-edit.hpp"
#include "duration-control.hpp"
#include "slider-spinbox.hpp"
#include "source-selection.hpp"

namespace advss {

class MacroActionMedia : public MacroAction {
public:
	MacroActionMedia(Macro *m) : MacroAction(m) {}
	static std::shared_ptr<MacroAction> Create(Macro *m)
	{
		return std::make_shared<MacroActionMedia>(m);
	}
	std::string GetId() const { return id; };
	std::string GetShortDesc() const;

	bool PerformAction();
	void LogAction() const;
	bool Save(obs_data_t *obj) const;
	bool Load(obs_data_t *obj);

	enum class Action {
		PLAY,
		PAUSE,
		STOP,
		RESTART,
		NEXT,
		PREVIOUS,
		SEEK_DURATION,
		SEEK_PERCENTAGE,
	};

	Action _action = Action::PLAY;
	Duration _seekDuration;
	DoubleVariable _seekPercentage = 50;
	SourceSelection _mediaSource;

private:
	void SeekToPercentage(obs_source_t *source) const;

	static bool _registered;
	static const std::string id;
};

class MacroActionMediaEdit : public QWidget {
	Q_OBJECT

public:
	MacroActionMediaEdit(
		QWidget *parent,
		std::shared_ptr<MacroActionMedia> entryData = nullptr);
	static QWidget *Create(QWidget *parent,
			       std::shared_ptr<MacroAction> action)
	{
		return new MacroActionMediaEdit(
			parent,
			std::dynamic_pointer_cast<MacroActionMedia>(action));
	}
	void UpdateEntryData();

private slots:
	void ActionChanged(int value);
	void SeekDurationChanged(const Duration &seekDuration);
	void
	SeekPercentageChanged(const NumberVariable<double> &seekPercentage);
	void SourceChanged(const SourceSelection &source);

signals:
	void HeaderInfoChanged(const QString &);

private:
	void SetWidgetVisibility();

	QComboBox *_actions;
	DurationSelection *_seekDuration;
	SliderSpinBox *_seekPercentage;
	SourceSelectionWidget *_sources;

	std::shared_ptr<MacroActionMedia> _entryData;
	bool _loading = true;
};

} // namespace advss
