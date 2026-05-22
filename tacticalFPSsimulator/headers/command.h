#pragma once
#include <functional>
#include <string>
#include <stack>
#include <memory>

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual bool canUndo() const { return false; }
    virtual void undo() {}
    virtual std::string description() const = 0;
};

class FunctionCommand : public ICommand {
    std::function<void()> action_;
    std::function<void()> undoAction_;
    std::string           desc_;

public:
    FunctionCommand(std::string desc,
        std::function<void()> action,
        std::function<void()> undoAction = nullptr);

    void execute() override;
    bool canUndo() const override;
    void undo()    override;
    std::string description() const override;
};

class CommandHistory {
    std::stack<std::unique_ptr<ICommand>> history_;

public:
    void execute(std::unique_ptr<ICommand> cmd);
    bool canUndo() const;
    void undo();
    void clear();
};