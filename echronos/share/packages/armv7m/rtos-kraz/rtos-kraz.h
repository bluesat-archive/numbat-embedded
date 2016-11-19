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
#ifndef RTOS_KRAZ_H
#define RTOS_KRAZ_H





#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <stdint.h>






typedef uint{{signalset_size}}_t {{prefix_type}}SignalSet;
typedef {{prefix_type}}SignalSet {{prefix_type}}SignalId;
{{#mutexes.length}}
typedef uint8_t {{prefix_type}}MutexId;
{{/mutexes.length}}
typedef uint8_t {{prefix_type}}ErrorId;
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;











#define {{prefix_const}}REENTRANT {{reentrant}}




#define {{prefix_const}}SIGNAL_SET_EMPTY (({{prefix_type}}SignalSet) UINT{{signalset_size}}_C(0))
#define {{prefix_const}}SIGNAL_SET_ALL (({{prefix_type}}SignalSet) UINT{{signalset_size}}_MAX)
{{#signal_sets}}
#define {{prefix_const}}SIGNAL_SET_{{name|u}} (({{prefix_type}}SignalSet) UINT{{signalset_size}}_C({{value}}))
{{#singleton}}#define {{prefix_const}}SIGNAL_ID_{{name|u}} (({{prefix_type}}SignalId) {{prefix_const}}SIGNAL_SET_{{name|u}}){{/singleton}}
{{/signal_sets}}
{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}

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
{{#mutexes.length}}
/*@unused@*/
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId) {{prefix_const}}REENTRANT;
/*@unused@*/
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
{{/mutexes.length}}

/*@unused@*/
{{prefix_type}}TaskId {{prefix_func}}task_current(void);
/*@unused@*/
void {{prefix_func}}yield(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}start(void);
#ifdef __cplusplus
}
#endif

#endif /* RTOS_KRAZ_H */