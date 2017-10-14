#pragma once

#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace vrlk::ArgParse {

// forward declarations
class ArgumentParser;
class ArgParseError;

// typedefs
using ParseResult =
    std::pair<std::unordered_map<std::string, std::vector<std::string>>,
              ArgParseError>;

enum class ArgumentType { Positional, Optional };

class ArgParseError {
    std::string message_ = "";
    bool failure_ = false;
    void setMessage(const std::string& m);
    void setFailure(bool b);

public:
    std::string message() const;

    bool failure() const;
    explicit operator bool() const;

    void setError(const std::string& s);
};

class ArgumentSpec {
    ArgumentParser& parser_;

    // fields
    std::string key_ = "";
    std::string longName_ = "";
    std::string shortName_ = "";
    ArgumentType type_ = ArgumentType::Positional;
    std::string nargs_ = "";
    bool required_ = false;

public:
    ArgumentSpec(ArgumentParser& p, const std::string& key);

    std::string key() const;
    void setKey(const std::string& s);
    ArgumentSpec& key(const std::string& k);

    std::string longName() const;
    void setLongName(const std::string& s);
    ArgumentSpec& longName(const std::string& n);

    std::string shortName() const;
    void setShortName(const std::string& s);
    ArgumentSpec& shortName(const std::string& s);

    ArgumentType type() const;
    void setType(ArgumentType t);
    ArgumentSpec& type(ArgumentType t);

    std::string nargs() const;
    void setNargs(const std::string& s);
    ArgumentSpec& nargs(const std::string& s);

    bool required() const;
    void setRequired(bool b);
    ArgumentSpec& required(bool b);

    // save this ArgumentSpec to belonging ArgumentParser
    // and canonicalize parameters
    ArgParseError save();
};

class ArgumentParser {
    std::queue<std::shared_ptr<ArgumentSpec>> positionals_;
    std::unordered_map<std::string, std::shared_ptr<ArgumentSpec>> optionals_;
    std::unordered_map<std::string, std::shared_ptr<ArgumentSpec>>
        requiredOptionals_;
    std::vector<std::string> names_;
    ArgParseError error_;
    bool invalid_ = false;

public:
    ArgumentSpec addArgument(const std::string& key);
    ParseResult parseArgs(int argc, char** argv);

    // save ArgumentSpec to me
    // and check parameter consistency
    ArgParseError saveSpec(const ArgumentSpec& spec);
};

} // namespace vrlk::ArgParse
