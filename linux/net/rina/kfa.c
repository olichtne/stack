/*
 * KFA (Kernel Flow Allocator)
 *
 *    Francesco Salvestrini <f.salvestrini@nextworks.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>

#define RINA_PREFIX "kfa"

#include "logs.h"
#include "debug.h"
#include "utils.h"
#include "fidm.h"
#include "kfa.h"

struct kfa {
        spinlock_t    lock;
        struct fidm * fidm;
};

struct kfa * kfa_create(void)
{
        struct kfa * instance;

        instance = rkzalloc(sizeof(*instance), GFP_KERNEL);
        if (!instance)
                return NULL;
        
        instance->fidm = fidm_create();
        if (!instance->fidm) {
                rkfree(instance);
                return NULL;
        }

        spin_lock_init(&instance->lock);

        return instance;
}

int kfa_destroy(struct kfa * instance)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return -1;
        }

        fidm_destroy(instance->fidm);
        rkfree(instance);

        return 0;
}

struct ipcp_flow {
        struct efcp * efcp;

        /* FIXME: Move KIPCM struct ipcp_flow here */
};

flow_id_t kfa_flow_create(struct kfa * instance)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return -1;
        }

        spin_lock(&instance->lock);
        LOG_MISSING;
        spin_unlock(&instance->lock);

        return flow_id_bad();
}
EXPORT_SYMBOL(kfa_flow_create);

int kfa_flow_bind(struct kfa * instance,
                  flow_id_t    fid,
                  port_id_t    pid)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return -1;
        }
        if (!is_flow_id_ok(fid)) {
                LOG_ERR("Bogus flow-id, bailing out");
                return -1;
        }
        if (!is_port_id_ok(fid)) {
                LOG_ERR("Bogus port-id, bailing out");
                return -1;
        }

        spin_lock(&instance->lock);
        LOG_MISSING;
        spin_unlock(&instance->lock);

        return -1;
}
EXPORT_SYMBOL(kfa_flow_bind);

flow_id_t kfa_flow_unbind(struct kfa * instance,
                          port_id_t    id)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return -1;
        }
        if (!is_port_id_ok(id)) {
                LOG_ERR("Bogus port-id, bailing out");
                return -1;
        }

        spin_lock(&instance->lock);
        LOG_MISSING;
        spin_unlock(&instance->lock);

        return flow_id_bad();
}
EXPORT_SYMBOL(kfa_flow_unbind);

int kfa_flow_destroy(struct kfa * instance,
                     flow_id_t    id)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return -1;
        }
        if (!is_flow_id_ok(id)) {
                LOG_ERR("Bogus flow-id, bailing out");
                return -1;
        }

        spin_lock(&instance->lock);
        LOG_MISSING;
        spin_unlock(&instance->lock);

        return -1;
}
EXPORT_SYMBOL(kfa_flow_destroy);

int kfa_flow_sdu_write(struct kfa *  instance,
                       port_id_t     id,
                       struct sdu *  sdu)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return -1;
        }
        if (!is_port_id_ok(id)) {
                LOG_ERR("Bogus port-id, bailing out");
                return -1;
        }
        if (!is_sdu_ok(sdu)) {
                LOG_ERR("Bogus port-id, bailing out");
                return -1;
        }

        spin_lock(&instance->lock);
        LOG_MISSING;
        spin_unlock(&instance->lock);

        return -1;
}
EXPORT_SYMBOL(kfa_flow_sdu_write);

struct sdu * kfa_flow_sdu_read(struct kfa *  instance,
                               port_id_t     id)
{
        if (!instance) {
                LOG_ERR("Bogus instance passed, bailing out");
                return NULL;
        }
        if (!is_port_id_ok(id)) {
                LOG_ERR("Bogus port-id, bailing out");
                return NULL;
        }

        spin_lock(&instance->lock);
        LOG_MISSING;
        spin_unlock(&instance->lock);

        return NULL;
}
EXPORT_SYMBOL(kfa_flow_sdu_read);
