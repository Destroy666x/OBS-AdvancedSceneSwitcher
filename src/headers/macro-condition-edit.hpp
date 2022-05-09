#pragma once
#include "macro-condition.hpp"

#include <memory>

struct MacroConditionInfo {
	using TCreateMethod = std::shared_ptr<MacroCondition> (*)(Macro *m);
	using TCreateWidgetMethod =
		QWidget *(*)(QWidget *parent, std::shared_ptr<MacroCondition>);
	TCreateMethod _createFunc = nullptr;
	TCreateWidgetMethod _createWidgetFunc = nullptr;
	std::string _name;
	bool _useDurationConstraint = true;
};

class MacroConditionFactory {
public:
	MacroConditionFactory() = delete;
	static bool Register(const std::string &, MacroConditionInfo);
	static std::shared_ptr<MacroCondition> Create(const std::string &,
						      Macro *m);
	static QWidget *CreateWidget(const std::string &id, QWidget *parent,
				     std::shared_ptr<MacroCondition>);
	static auto GetConditionTypes() { return _methods; }
	static std::string GetConditionName(const std::string &);
	static std::string GetIdByName(const QString &name);
	static bool UsesDurationConstraint(const std::string &id);

private:
	static std::map<std::string, MacroConditionInfo> _methods;
};

class DurationConstraintEdit : public QWidget {
	Q_OBJECT
public:
	DurationConstraintEdit(QWidget *parent = nullptr);
	void SetValue(DurationConstraint &value);
	void SetUnit(DurationUnit u);
	void SetDuration(const Duration &d);

private slots:
	void _ConditionChanged(int value);
	void ToggleClicked();
signals:
	void DurationChanged(double value);
	void UnitChanged(DurationUnit u);
	void ConditionChanged(DurationCondition value);

private:
	void Collapse(bool collapse);

	DurationSelection *_duration;
	QComboBox *_condition;
	QPushButton *_toggle;
};

class MacroConditionEdit : public MacroSegmentEdit {
	Q_OBJECT

public:
	MacroConditionEdit(QWidget *parent = nullptr,
			   std::shared_ptr<MacroCondition> * = nullptr,
			   const std::string &id = "scene", bool root = true);
	bool IsRootNode();
	void SetRootNode(bool);
	void UpdateEntryData(const std::string &id);
	void SetEntryData(std::shared_ptr<MacroCondition> *);

private slots:
	void LogicSelectionChanged(int idx);
	void ConditionSelectionChanged(const QString &text);
	void DurationChanged(double seconds);
	void DurationConditionChanged(DurationCondition cond);
	void DurationUnitChanged(DurationUnit unit);

private:
	void SetLogicSelection();
	MacroSegment *Data();

	QComboBox *_logicSelection;
	QComboBox *_conditionSelection;
	DurationConstraintEdit *_dur;

	std::shared_ptr<MacroCondition> *_entryData;
	bool _isRoot = true;
	bool _loading = true;
};
