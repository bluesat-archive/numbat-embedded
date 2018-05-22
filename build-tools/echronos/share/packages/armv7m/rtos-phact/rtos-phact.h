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
#ifndef RTOS_PHACT_H
#define RTOS_PHACT_H




#include <stdbool.h>
#include <stdint.h>

#include <stdint.h>

{{#interrupt_events.length}}
#include <stdint.h>
{{/interrupt_events.length}}

#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <stdint.h>





typedef uint{{signalset_size}}_t {{prefix_type}}SignalSet;
typedef {{prefix_type}}SignalSet {{prefix_type}}SignalId;

typedef uint8_t {{prefix_type}}TimerId;
typedef uint32_t {{prefix_type}}TicksAbsolute;
typedef uint16_t {{prefix_type}}TicksRelative;

{{#interrupt_events.length}}
typedef uint{{interrupteventid_size}}_t {{prefix_type}}InterruptEventId;
{{/interrupt_events.length}}

typedef uint8_t {{prefix_type}}MutexId;
typedef uint8_t {{prefix_type}}SemId;
typedef uint{{semaphore_value_size}}_t {{prefix_type}}SemValue;
typedef uint8_t {{prefix_type}}ErrorId;
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;
















#define {{prefix_const}}REENTRANT {{reentrant}}



#define {{prefix_const}}SIGNAL_SET_EMPTY (({{prefix_type}}SignalSet) UINT{{signalset_size}}_C(0))
#define {{prefix_const}}SIGNAL_SET_ALL (({{prefix_type}}SignalSet) UINT{{signalset_size}}_MAX)
{{#signal_sets}}
#define {{prefix_const}}SIGNAL_SET_{{name|u}} (({{prefix_type}}SignalSet) UINT{{signalset_size}}_C({{value}}))
{{#singleton}}#define {{prefix_const}}SIGNAL_ID_{{name|u}} (({{prefix_type}}SignalId) {{prefix_const}}SIGNAL_SET_{{name|u}}){{/singleton}}
{{/signal_sets}}

{{#timers}}
#define {{prefix_const}}TIMER_ID_{{name|u}} (({{prefix_type}}TimerId) UINT8_C({{idx}}))
{{/timers}}

{{#interrupt_events}}
#define {{prefix_const}}INTERRUPT_EVENT_ID_{{name|u}} (({{prefix_type}}InterruptEventId) UINT{{interrupteventid_size}}_C({{idx}}))
{{/interrupt_events}}

{{#mutexes.length}}
#define {{prefix_const}}MUTEX_ID_ZERO (({{prefix_type}}MutexId) UINT8_C(0))
#define {{prefix_const}}MUTEX_ID_MAX (({{prefix_type}}MutexId) UINT8_C({{mutexes.length}} - 1))
{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}
{{/mutexes.length}}
#define {{prefix_const}}SEM_ID_ZERO (({{prefix_type}}SemId) UINT8_C(0))
#define {{prefix_const}}SEM_ID_MAX (({{prefix_type}}SemId) UINT8_C({{semaphores.length}} - 1))
{{#semaphores}}
#define {{prefix_const}}SEM_ID_{{name|u}} (({{prefix_type}}SemId) UINT8_C({{idx}}))
{{/semaphores}}

#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT{{taskid_size}}_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT{{taskid_size}}_C({{tasks.length}} - 1))
{{#tasks}}
#define {{prefix_const}}TASK_ID_{{name|u}} (({{prefix_type}}TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}





#define {{prefix_func}}signal_wait(requested_signal) \
    (void) {{prefix_func}}signal_wait_set(requested_signal)

#define {{prefix_func}}signal_poll(requested_signal) \
    ({{prefix_func}}signal_poll_set(requested_signal) != {{prefix_const}}SIGNAL_SET_EMPTY)

#define {{prefix_func}}signal_peek(requested_signal) \
    ({{prefix_func}}signal_peek_set(requested_signal) != {{prefix_const}}SIGNAL_SET_EMPTY)

#define {{prefix_func}}signal_send(task_id, signal) \
    {{prefix_func}}signal_send_set(task_id, signal)
















/*@unused@*/
extern {{prefix_type}}TicksAbsolute {{prefix_func}}timer_current_ticks;



{{#mutex.stats}}
/*@unused@*/
extern bool {{prefix_func}}mutex_stats_enabled;
{{/mutex.stats}}




#ifdef __cplusplus
extern "C" {
#endif




/*@unused@*/
{{prefix_type}}SignalSet {{prefix_func}}signal_wait_set({{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT;
/*@unused@*/
{{prefix_type}}SignalSet {{prefix_func}}signal_poll_set({{prefix_type}}SignalSet requested_signals);
/*@unused@*/
{{prefix_type}}SignalSet {{prefix_func}}signal_peek_set({{prefix_type}}SignalSet requested_signals);
/*@unused@*/
void {{prefix_func}}signal_send_set({{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signals);
/*@unused@*/
void {{prefix_func}}timer_tick(void);
/*@unused@*/
void {{prefix_func}}sleep({{prefix_type}}TicksRelative ticks) {{prefix_const}}REENTRANT;
{{#timers.length}}
/*@unused@*/
void {{prefix_func}}timer_enable({{prefix_type}}TimerId timer_id);
/*@unused@*/
void {{prefix_func}}timer_disable({{prefix_type}}TimerId timer_id);
/*@unused@*/
void {{prefix_func}}timer_oneshot({{prefix_type}}TimerId timer_id, {{prefix_type}}TicksRelative timeout);
/*@unused@*/
bool {{prefix_func}}timer_check_overflow({{prefix_type}}TimerId timer_id);
/*@unused@*/
{{prefix_type}}TicksRelative {{prefix_func}}timer_remaining({{prefix_type}}TimerId timer_id);
/*@unused@*/
void {{prefix_func}}timer_reload_set({{prefix_type}}TimerId timer_id, {{prefix_type}}TicksRelative reload);
/*@unused@*/
void {{prefix_func}}timer_error_set({{prefix_type}}TimerId timer_id, {{prefix_type}}ErrorId error_id);
/*@unused@*/
void {{prefix_func}}timer_signal_set({{prefix_type}}TimerId timer_id, {{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signal_set);
{{/timers.length}}
{{#interrupt_events.length}}
void {{prefix_func}}interrupt_event_raise({{prefix_type}}InterruptEventId event);
{{/interrupt_events.length}}


{{#mutexes.length}}
/*@unused@*/
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId) {{prefix_const}}REENTRANT;
/*@unused@*/
bool {{prefix_func}}mutex_lock_timeout({{prefix_type}}MutexId, {{prefix_type}}TicksRelative) {{prefix_const}}REENTRANT;
/*@unused@*/
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
/*@unused@*/
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
/*@unused@*/
bool {{prefix_func}}mutex_holder_is_current({{prefix_type}}MutexId);
{{#mutex.stats}}
/*@unused@*/
void {{prefix_func}}mutex_stats_clear(void);
{{/mutex.stats}}
{{/mutexes.length}}
/*@unused@*/
void {{prefix_func}}sem_post({{prefix_type}}SemId);
/*@unused@*/
bool {{prefix_func}}sem_try_wait({{prefix_type}}SemId);
/*@unused@*/
void {{prefix_func}}sem_wait({{prefix_type}}SemId) {{prefix_const}}REENTRANT;
/*@unused@*/
bool {{prefix_func}}sem_wait_timeout({{prefix_type}}SemId, {{prefix_type}}TicksRelative timeout)
        {{prefix_const}}REENTRANT;
{{#semaphore_enable_max}}
void {{prefix_func}}sem_max_init({{prefix_type}}SemId s, {{prefix_type}}SemValue max);
{{/semaphore_enable_max}}

/*@unused@*/
{{prefix_type}}TaskId {{prefix_func}}task_current(void);
void {{prefix_func}}start(void);
#ifdef __cplusplus
}
#endif

#endif /* RTOS_PHACT_H */