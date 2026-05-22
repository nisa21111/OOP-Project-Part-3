#include "command.h"
#include <iostream>

// ─── FunctionCommand ───────────────────────────────────────────────────────────

FunctionCommand::FunctionCommand(std::string desc,
    std::function<void()> action,
    std::function<void()> undoAction)
    : action_(std::move(action))
    , undoAction_(std::move(undoAction))
    , desc_(std::move(desc))
{
}

void FunctionCommand::execute() { if (action_) action_(); }
bool FunctionCommand::canUndo() const { return undoAction_ != nullptr; }
void FunctionCommand::undo() { if (undoAction_) undoAction_(); }
std::string FunctionCommand::description() const { return desc_; }

// ─── CommandHistory ────────────────────────────────────────────────────────────

void CommandHistory::execute(std::unique_ptr<ICommand> cmd) {
    cmd->execute();
    if (cmd->canUndo()) history_.push(std::move(cmd));
}

bool CommandHistory::canUndo() const { return !history_.empty(); }

void CommandHistory::undo() {
    if (history_.empty()) return;
    history_.top()->undo();
    history_.pop();
}

void CommandHistory::clear() {
    while (!history_.empty()) history_.pop();
}