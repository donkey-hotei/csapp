#
# Solution to CMU's bomb lab using symbolic execution.
#
import angr, logging

def solve_phase_one():
    # Phase 1
    phase_one_explode = 0x00400ef2
    return

def solve_phase_two():
    # Phase 2
    opts = {"auto_load_libs": False}
    proj = angr.Project("bomb", load_options=opts)

    phase_two_explode = 0x00400f10
    phase_two_success = 0x00400f3c

    addr  = 0x00400f0a  # address after `call read_six_numbers`
    state = proj.factory.blank_state(addr=addr)

    # Push the values onto the stack that would have been read
    # by `read_six_numbers`, as symbolic 32-bit values.
    for x in range(6):
        state.stack_push(state.se.BVS('int%d' % x, 32))

    path = proj.factory.path(state=state)
    expl = proj.surveyors.Explorer(start=path,
                                   find=(phase_two_success,),
                                   avoid=(phase_two_explode,),
                                   enable_veritesting=True)
    expl.run()

    if expl.found:
        found = expl.found[0].state
        solution = []
        for _ in range(3):
            current_int = found.se.any_int(found.stack_pop())
            solution.append(str(current_int & 0xffffffff))
            solution.append(str(current_int >> 32 & 0xffffffff))

    print("Phase 2 solution: %s" % ''.join(solution))
    return

def solve_phase_three():
    # Phase 3
    phase_three_explode = 0x401035
    return

if __name__ == "__main__":
    solve_phase_one()
    solve_phase_two()
    solve_phase_three()