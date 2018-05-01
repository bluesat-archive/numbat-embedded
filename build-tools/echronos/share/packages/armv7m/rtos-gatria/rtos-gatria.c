/*
 * eChronos Real-Time Operating System
 * Copyright (C) 2015  National ICT Australia Limited (NICTA), ABN 62 102 206 173.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, version 3, provided that these additional
 * terms apply under section 7:
 *
 *   No right, title or interest in or to any trade mark, service mark, logo
 *   or trade name of of National ICT Australia Limited, ABN 62 102 206 173
 *   ("NICTA") or its licensors is granted. Modified versions of the Program
 *   must be plainly marked as such, and must not be distributed using
 *   "eChronos" as a trade mark or product name, or misrepresented as being
 *   the original Program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>

#include <stdbool.h>
{{#mutexes.length}}
#include <stdbool.h>
{{/mutexes.length}}
#include <stdint.h>
#include <stdint.h>
#include "rtos-gatria.h"


#define CONTEXT_SIZE 10
#define CONTEXT_V1_IDX 0
#define CONTEXT_V2_IDX 1
#define CONTEXT_V3_IDX 2
#define CONTEXT_V4_IDX 3
#define CONTEXT_V5_IDX 4
#define CONTEXT_V6_IDX 5
#define CONTEXT_V7_IDX 6
#define CONTEXT_V8_IDX 7
#define CONTEXT_IP_IDX 8
#define CONTEXT_PC_IDX 9



#define ERROR_ID_NONE (({{prefix_type}}ErrorId) UINT8_C(0))
#define ERROR_ID_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(1))
#define ERROR_ID_INVALID_ID (({{prefix_type}}ErrorId) UINT8_C(2))
#define ERROR_ID_NOT_HOLDING_MUTEX (({{prefix_type}}ErrorId) UINT8_C(3))
#define ERROR_ID_DEADLOCK (({{prefix_type}}ErrorId) UINT8_C(4))
#define ERROR_ID_TASK_FUNCTION_RETURNS (({{prefix_type}}ErrorId) UINT8_C(5))
#define ERROR_ID_INTERNAL_CURRENT_TASK_INVALID (({{prefix_type}}ErrorId) UINT8_C(6))
#define ERROR_ID_INTERNAL_INVALID_ID (({{prefix_type}}ErrorId) UINT8_C(7))
#define ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP (({{prefix_type}}ErrorId) UINT8_C(8))
#define ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT (({{prefix_type}}ErrorId) UINT8_C(9))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT (({{prefix_type}}ErrorId) UINT8_C(10))
#define ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER (({{prefix_type}}ErrorId) UINT8_C(11))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(12))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION (({{prefix_type}}ErrorId) UINT8_C(13))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION (({{prefix_type}}ErrorId) UINT8_C(14))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD (({{prefix_type}}ErrorId) UINT8_C(15))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE (({{prefix_type}}ErrorId) UINT8_C(16))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS (({{prefix_type}}ErrorId) UINT8_C(17))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES (({{prefix_type}}ErrorId) UINT8_C(18))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED (({{prefix_type}}ErrorId) UINT8_C(19))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER (({{prefix_type}}ErrorId) UINT8_C(20))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE (({{prefix_type}}ErrorId) UINT8_C(21))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH (({{prefix_type}}ErrorId) UINT8_C(22))
#define ERROR_ID_INTERNAL_PRECONDITION_VIOLATED (({{prefix_type}}ErrorId) UINT8_C(23))
#define ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED (({{prefix_type}}ErrorId) UINT8_C(24))
#define ERROR_ID_SEMAPHORE_MAX_INVALID (({{prefix_type}}ErrorId) UINT8_C(25))
#define ERROR_ID_SEMAPHORE_MAX_USE_BEFORE_INIT (({{prefix_type}}ErrorId) UINT8_C(26))
#define ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT (({{prefix_type}}ErrorId) UINT8_C(27))
#define ERROR_ID_SEMAPHORE_MAX_EXCEEDED (({{prefix_type}}ErrorId) UINT8_C(28))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED (({{prefix_type}}ErrorId) UINT8_C(29))
#define ERROR_ID_SCHED_PRIO_CEILING_TASK_LOCKING_LOWER_PRIORITY_MUTEX (({{prefix_type}}ErrorId) UINT8_C(30))
#define ERROR_ID_SCHED_PRIO_CEILING_MUTEX_ALREADY_LOCKED (({{prefix_type}}ErrorId) UINT8_C(31))
/* The TASK_ID_NONE and TASK_ID_END macros require some care:
 * - TASK_ID_NONE is a valid integer within the value range of the TaskIdOption/TaskId types.
 *   There is no fundamental safeguard against the application defining TASK_ID_NONE+1 tasks so that the last task
 *   receives a task ID that is numerically equal to TASK_ID_NONE.
 * - TASK_ID_END is of type integer, not TaskIdOption/TaskId.
 *   It may hold the value TASK_ID_MAX + 1 which potentially exceeds the valid value range of TaskIdOption/TaskId.
 *   It can therefore not necessarily be safely assigned to or cast to type TaskIdOption/TaskId. */
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)
#define TASK_ID_END ({{tasks.length}})
#define current_task rtos_internal_current_task
#define tasks rtos_internal_tasks

typedef uint32_t* context_t;
typedef {{prefix_type}}TaskId SchedIndex;
typedef {{prefix_type}}TaskId TaskIdOption;




struct sched_task {
    bool runnable;
};

struct sched {
    SchedIndex cur; /* The index of the currently scheduled task */
    struct sched_task tasks[{{tasks.length}}];
};
{{#mutexes.length}}
struct mutex {
    bool locked;
};
{{/mutexes.length}}

struct task
{
    context_t ctx;
};



extern void rtos_internal_context_switch(context_t *, context_t *) {{prefix_const}}REENTRANT;
extern void rtos_internal_context_switch_first(context_t *) {{prefix_const}}REENTRANT;
extern void rtos_internal_trampoline(void);



extern /*@noreturn@*/ void {{fatal_error}}({{prefix_type}}ErrorId error_id);
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}



/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
  *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The concept of a context and of the context_t type is abstract and may have different implementations on
 *  different platforms.
 *  It can be, e.g., a stack pointer or a data structure for user-level task switching as on POSIX.
 *  This function is expected to set ctx to a value that the RTOS can pass to this platform's implementation of
 *  context_switch() and context_switch_first().
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 *  For hardware platforms, this typically requires the following set up steps:
 *  - The value of ctx points to either the beginning or the end of the stack area.
 *  - The stack area contains fn so that the context-switch functions can pop it off the stack as a return address to
 *    begin execution at.
 *  - Optionally reserve additional stack space if the context-switch functions depend on it.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size);

static void sched_set_runnable(const {{prefix_type}}TaskId task_id);
static void sched_set_blocked(const {{prefix_type}}TaskId task_id);
static {{prefix_type}}TaskId sched_get_next(void);


{{#internal_asserts}}
static {{prefix_type}}TaskId get_current_task_check(void);
{{/internal_asserts}}


{{#tasks}}
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));
{{/tasks}}


static struct sched sched_tasks;
{{#mutexes.length}}
static struct mutex mutexes[{{mutexes.length}}];
{{/mutexes.length}}

/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
{{prefix_type}}TaskId rtos_internal_current_task;
/*@unused@ must be public so that packages/armv7m/ctxt-switch-preempt.s can access this symbol */
struct task rtos_internal_tasks[{{tasks.length}}];



#define context_switch(from, to) rtos_internal_context_switch(to, from)
#define context_switch_first(to) rtos_internal_context_switch_first(to)
#define preempt_disable()
#define preempt_enable()
#define preempt_clear()
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_next_index(cur) (((cur) == sched_max_index()) ? 0 : ((cur) + 1))
#define sched_get_cur_index() (sched_tasks.cur)
#define sched_set_cur_index(idx) sched_tasks.cur = (idx)
#define sched_max_index() (SchedIndex)({{tasks.length}} - 1U)
#define sched_index_to_taskid(sched_index) ({{prefix_type}}TaskId)(sched_index)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]
#define assert_mutex_valid(mutex) api_assert(mutex < {{mutexes.length}}, ERROR_ID_INVALID_ID)
{{#api_asserts}}
#define api_error(error_id) {{fatal_error}}(error_id)
{{/api_asserts}}
{{^api_asserts}}
#define api_error(error_id)
{{/api_asserts}}
{{#api_asserts}}
#define api_assert(expression, error_id) do { if (!(expression)) { api_error(error_id); } } while(0)
{{/api_asserts}}
{{^api_asserts}}
#define api_assert(expression, error_id)
{{/api_asserts}}

{{#internal_asserts}}
#define internal_error(error_id) {{fatal_error}}(error_id)
{{/internal_asserts}}
{{^internal_asserts}}
#define internal_error(error_id)
{{/internal_asserts}}
{{#internal_asserts}}
#define internal_assert(expression, error_id) do { if (!(expression)) { internal_error(error_id); } } while(0)
{{/internal_asserts}}
{{^internal_asserts}}
#define internal_assert(expression, error_id)
{{/internal_asserts}}
{{#internal_asserts}}
#define get_current_task() get_current_task_check()
{{/internal_asserts}}
{{^internal_asserts}}
#define get_current_task() current_task
{{/internal_asserts}}
#define get_task_context(task_id) &tasks[task_id].ctx
#define internal_assert_task_valid(task) internal_assert(task < {{tasks.length}}, ERROR_ID_INTERNAL_INVALID_ID)
#define assert_task_valid(task) api_assert(task < {{tasks.length}}, ERROR_ID_INVALID_ID)



static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *context;
    context = stack_base + stack_size - CONTEXT_SIZE;
    context[CONTEXT_V1_IDX] = (uint32_t) fn;
    context[CONTEXT_PC_IDX] = (uint32_t) rtos_internal_trampoline;
    *ctx = context;
}

static void
sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = true;
}

static void
sched_set_blocked(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = false;
}

static {{prefix_type}}TaskId
sched_get_next(void)
{
    {{prefix_type}}TaskId task;
    SchedIndex next = sched_get_cur_index();
    bool found = false;

    do
    {
        next = sched_next_index(next);
        found = sched_runnable(sched_index_to_taskid(next));
    } while (
        (!found)
        );

    {
        task = sched_index_to_taskid(next);
    }

    sched_set_cur_index(next);

    return task;
}
{{#mutexes.length}}
static bool
internal_mutex_try_lock(const {{prefix_type}}MutexId m)
{
    if (mutexes[m].locked)
    {
        return false;
    }
    else
    {
        mutexes[m].locked = true;
        return true;
    }
}
{{/mutexes.length}}

{{#internal_asserts}}
static {{prefix_type}}TaskId
get_current_task_check(void)
{
    internal_assert(current_task < {{tasks.length}}, ERROR_ID_INTERNAL_CURRENT_TASK_INVALID);
    return current_task;
}
{{/internal_asserts}}






{{#mutexes.length}}
void
{{prefix_func}}mutex_lock(const {{prefix_type}}MutexId m) {{prefix_const}}REENTRANT
{
    assert_mutex_valid(m);

    preempt_disable();

    while (!internal_mutex_try_lock(m))
    {
        {{prefix_func}}yield();
    }

    preempt_enable();
}

void
{{prefix_func}}mutex_unlock(const {{prefix_type}}MutexId m)
{
    assert_mutex_valid(m);

    /* Note: assumes writing a single word is atomic */
    mutexes[m].locked = false;
}

bool
{{prefix_func}}mutex_try_lock(const {{prefix_type}}MutexId m)
{
    bool r;

    assert_mutex_valid(m);

    preempt_disable();
    r = internal_mutex_try_lock(m);
    preempt_enable();

    return r;
}
{{/mutexes.length}}

{{prefix_type}}TaskId
{{prefix_func}}task_current(void)
{
    return get_current_task();
}
void
{{prefix_func}}yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();
    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId to = sched_get_next();
    {{prefix_func}}yield_to(to);
}

void
{{prefix_func}}block(void) {{prefix_const}}REENTRANT
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

void
{{prefix_func}}unblock(const {{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

void
{{prefix_func}}start(void)
{
    {{#tasks}}
    context_init(get_task_context({{idx}}), {{function}}, stack_{{idx}}, {{stack_size}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
