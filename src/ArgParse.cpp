#include "ArgParse.hpp"

#include "String.hpp"

#include <algorithm>
#include <cassert>

namespace {
std::vector<std::string> convertProgramArguments(int argc, char** argv)
{
    std::vector<std::string> r;
    for (auto i = 1; i < argc; ++i) {
        r.emplace_back(argv[i]);
    }
    return r;
}
} // unnamed namespace

namespace vrlk::ArgParse {

//// ArgParseError

std::string ArgParseError::message() const
{
    return message_;
}

void ArgParseError::setMessage(const std::string& m)
{
    message_ = m;
}

bool ArgParseError::failure() const
{
    return failure_;
}

ArgParseError::operator bool() const
{
    return failure();
}

void ArgParseError::setFailure(bool b)
{
    failure_ = b;
}

void ArgParseError::setError(const std::string& s)
{
    setFailure(true);
    setMessage(s);
}

//// ArgParseError

//// ArgumentSpec

ArgumentSpec::ArgumentSpec(ArgumentParser& p, const std::string& key)
    : parser_{p}
    , key_{key}
{
}

std::string ArgumentSpec::key() const
{
    return key_;
}

void ArgumentSpec::setKey(const std::string& s)
{
    key_ = s;
}

std::string ArgumentSpec::longName() const
{
    return longName_;
}

void ArgumentSpec::setLongName(const std::string& s)
{
    longName_ = s;
}

std::string ArgumentSpec::shortName() const
{
    return shortName_;
}

void ArgumentSpec::setShortName(const std::string& s)
{
    shortName_ = s;
}

ArgumentType ArgumentSpec::type() const
{
    return type_;
}

void ArgumentSpec::setType(ArgumentType t)
{
    type_ = t;
}

std::string ArgumentSpec::nargs() const
{
    return nargs_;
}

void ArgumentSpec::setNargs(const std::string& s)
{
    nargs_ = s;
}

ArgumentSpec& ArgumentSpec::key(const std::string& k)
{
    setKey(k);
    return *this;
}

ArgumentSpec& ArgumentSpec::longName(const std::string& n)
{
    setLongName(n);
    return *this;
}

ArgumentSpec& ArgumentSpec::shortName(const std::string& s)
{
    setShortName(s);
    return *this;
}

ArgumentSpec& ArgumentSpec::type(ArgumentType t)
{
    setType(t);
    return *this;
}

ArgumentSpec& ArgumentSpec::nargs(const std::string& s)
{
    setNargs(s);
    return *this;
}

bool ArgumentSpec::required() const
{

    return required_;
}

void ArgumentSpec::setRequired(bool b)
{
    required_ = b;
}

ArgumentSpec& ArgumentSpec::required(bool b)
{
    setRequired(b);
    return *this;
}

ArgParseError ArgumentSpec::save()
{
    assert(!key().empty());

    // if long name is not set then key is used as long name
    if (longName().empty()) {
        setLongName(key());
    }

    if (nargs().empty()) {
        if (type() == ArgumentType::Optional) {
            // optional argument is used as switch by default
            setNargs("0");
        }
        else {
            // positional argument eats one item by default
            setNargs("1");
        }
    }

    return parser_.saveSpec(*this);
}

//// ArgumentSpec

//// ArgumentParser

ArgumentSpec ArgumentParser::addArgument(const std::string& key)
{
    return ArgumentSpec{*this, key};
}

ParseResult ArgumentParser::parseArgs(int argc, char** argv)
{
    // TODO: error handling
    auto programArguments = convertProgramArguments(argc, argv);

    ParseResult r;
    if (invalid_) {
        r.second = error_;
        return r;
    }

    // TODO: structured bindings
    auto&& m = r.first;
    auto&& error = r.second;

    auto isOptional = [](const std::string& s) {
        return String::startsWidth(s, "-");
    };
    auto optionName = [isOptional](const std::string& s) {
        assert(isOptional(s));

        if (String::startsWidth(s, "--")) {
            return s.substr(2);
        }
        else {
            return s.substr(1);
        }
    };

    std::shared_ptr<ArgumentSpec> currentOptional;
    std::shared_ptr<ArgumentSpec> currentPositional;
    std::vector<std::string> temp;
    int nargs = 0;

    if (!positionals_.empty()) {
        currentPositional = positionals_.front();
        positionals_.pop();
    }

    for (auto&& i : programArguments) {
        auto optional = isOptional(i);

        if (currentOptional != nullptr && optional) {
            error.setError("Unexpected optional argument while parsing " +
                           currentOptional->key() + ": " + i);
            return r;
        }
        else if (optional) {
            auto name = optionName(i);
            auto rit = requiredOptionals_.find(name);
            auto nrit = optionals_.find(name);

            // there is no such optional argument
            if (rit == std::end(requiredOptionals_) &&
                nrit == std::end(optionals_)) {
                error.setError("Unrecognized optional argument: " + i);
                return r;
            }
            // if this argument is marked as required then remove from list
            else if (rit != std::end(requiredOptionals_)) {
                currentOptional = rit->second;
                requiredOptionals_.erase(currentOptional->longName());
                if (!currentOptional->shortName().empty()) {
                    requiredOptionals_.erase(currentOptional->shortName());
                }
            }
            else {
                currentOptional = nrit->second;
            }

            auto n = std::stoi(currentOptional->nargs());
            if (n == 0) {
                m[currentOptional->key()] = {"true"};
                currentOptional = nullptr;
            }
            else {
                nargs = n;
            }
        }
        else if (currentOptional != nullptr) {
            temp.push_back(i);
            nargs -= 1;
            if (nargs == 0) {
                m[currentOptional->key()] = temp;
                currentOptional = nullptr;
                temp = {};
            }
        }
        else if (currentPositional != nullptr) {
            m[currentPositional->key()] = {i};
            if (!positionals_.empty()) {
                currentPositional = positionals_.front();
                positionals_.pop();
            }
            else {
                currentPositional = nullptr;
            }
        }
    }

    if (currentPositional != nullptr) {
        error.setError("Missing positional argument: " +
                       currentPositional->key());
        return r;
    }

    if (!requiredOptionals_.empty()) {
        std::vector<std::string> missing;
        for (auto&& i : requiredOptionals_) {
            auto&& n = i.second->key();
            if (std::find(std::begin(missing), std::end(missing), n) ==
                std::end(missing)) {
                missing.push_back(n);
            }
        }
        error.setError("Missing required argument: " +
                       String::join(", ", missing));
        return r;
    }

    if (currentOptional != nullptr) {
        error.setError(currentOptional->key() + " requires " +
                       std::to_string(nargs) + " more arguments");
        return r;
    }

    return r;
}

// TODO: separate optional and positional
// TODO: check nargs
ArgParseError ArgumentParser::saveSpec(const ArgumentSpec& spec)
{
    if (invalid_) {
        return error_;
    }

    assert(!spec.key().empty());
    assert(!spec.longName().empty());

    auto checkDuplicatedName = [this](const ArgumentSpec& s) {
        ArgParseError r;
        if (std::find(std::begin(names_), std::end(names_), s.key()) !=
            std::end(names_)) {
            r.setError("Key duplicated: " + s.key());
            return r;
        }
        if (s.type() == ArgumentType::Optional) {
            if (std::find(std::begin(names_), std::end(names_), s.longName()) !=
                std::end(names_)) {
                r.setError("Long name duplicated: " + s.longName());
                return r;
            }
            if (!s.shortName().empty()&&
                (std::find(std::begin(names_), std::end(names_),
                           s.shortName()) != std::end(names_))) {
                r.setError("Short name duplicated: " + s.shortName());
                return r;
            }
        }
        return r;
    };

    auto addOptional =
        [&spec](const std::shared_ptr<ArgumentSpec>& p,
                std::unordered_map<std::string, std::shared_ptr<ArgumentSpec>>&
                    target) {
            if (!spec.shortName().empty()) {
                target[spec.shortName()] = p;
            }
            target[spec.longName()] = p;
        };

    auto saveName = [this](const ArgumentSpec& s) {
        names_.push_back(s.key());
        if (s.type() == ArgumentType::Optional) {
            if (s.longName() != s.key()) {
                names_.push_back(s.longName());
            }
            if (!s.shortName().empty()) {
                names_.push_back(s.shortName());
            }
        }
    };

    auto sr = checkDuplicatedName(spec);
    if (sr) {
        invalid_ = true;
        error_ = sr;
        return sr;
    }

    auto p = std::make_shared<ArgumentSpec>(spec);
    if (spec.type() == ArgumentType::Positional) {
        positionals_.push(p);
    }
    else if (spec.required()) {
        addOptional(p, requiredOptionals_);
    }
    else {
        addOptional(p, optionals_);
    }
    saveName(spec);
    return ArgParseError{};
}

//// ArgumentParser

} // namespace vrlk::ArgParse
