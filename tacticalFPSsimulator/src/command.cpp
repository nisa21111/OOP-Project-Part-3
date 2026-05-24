#include "command.h"
#include <iostream>


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
