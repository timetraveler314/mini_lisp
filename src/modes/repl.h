//
// Created by timetraveler314 on 5/18/24.
//

#ifndef MINI_LISP_REPL_H
#define MINI_LISP_REPL_H

#include "../eval_env.h"

void startRepl(std::istream& in, std::ostream& out, std::shared_ptr<std::ostream> save, const std::shared_ptr<EvalEnv>& env, bool interactive = false);

#endif //MINI_LISP_REPL_H
