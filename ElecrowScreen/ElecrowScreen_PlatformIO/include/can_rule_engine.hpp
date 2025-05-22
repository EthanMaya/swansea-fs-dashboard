#ifndef _CAN_RULE_ENGINE_HPP
#define _CAN_RULE_ENGINE_HPP

#include <functional>
#include <stdio.h>
#include <utility>
#include <vector>

template <typename Ctx> struct Rule {
  using Condition = std::function<bool(const Ctx &)>;
  using Action = std::function<void(const Ctx &)>;

  Condition when; // “if …”
  Action then;    // “… do …”

  Rule(Condition c, Action a) : when(std::move(c)), then(std::move(a)) {}
};

template <typename Ctx> class RuleEngine {
  std::vector<Rule<Ctx>> rules_;

public:
  template <class Cond, class Act> void add_rule(Cond &&c, Act &&a) {
    rules_.emplace_back(std::forward<Cond>(c), std::forward<Act>(a));
  }

  void run(const Ctx &ctx) const {
    for (auto const &r : rules_) {
      if (r.when(ctx))
        r.then(ctx);
    }
  }
};

#endif
