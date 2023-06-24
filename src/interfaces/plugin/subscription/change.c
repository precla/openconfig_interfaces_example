/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "change.h"

int extract_xpath_key_value(const char *xpath, const char *list, const char *key, char *buffer, size_t buffer_size)
{
    int error = 0;

    const char *name = NULL;
    char *xpath_copy = NULL;

    sr_xpath_ctx_t xpath_ctx = {0};

    // copy xpath due to changing it when using xpath_ctx from sysrepo
    xpath_copy = strdup(xpath);
    if (!xpath_copy)
        goto error_out;

    // extract key
    name = sr_xpath_key_value(xpath_copy, list, key, &xpath_ctx);
    if (!name)
        goto error_out;

    // store to buffer
    if (snprintf(buffer, buffer_size, "%s", name) < 0) {
        goto error_out;
    }

    error = 0;
    goto out;

error_out:
    error = -1;
out:
    if (xpath_copy)
        free(xpath_copy);

    return error;
}

int iterate_changes(void *priv, sr_session_ctx_t *session, const char *xpath, int (*cb)(const change_ctx_t *change_ctx))
{
    int error = 0;
    int counter = 1;

    sr_change_iter_t *changes_iterator = NULL;
    change_ctx_t change_ctx;

    error = sr_get_changes_iter(session, xpath, &changes_iterator);
    if (error != SR_ERR_OK) {
        error = 2;
        goto out;
    }

    while (sr_get_change_tree_next(session, changes_iterator, &change_ctx.operation, &change_ctx.node,
                                   &change_ctx.previous_value, &change_ctx.previous_list,
                                   &change_ctx.previous_default) == SR_ERR_OK) {
        error = cb(&change_ctx);
        if (error) {
            // return number of invalid callback
            error = -counter;
            goto out;
        }
        ++counter;
    }

out:
    sr_free_change_iter(changes_iterator);
    return error;
}

int interfaces_interface_ipv4_change_dhcp_client(const change_ctx_t *change_ctx)
{
	int error = 0;

	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	char path_buffer[PATH_MAX] = { 0 };
	char interface_name_buffer[IFNAMSIZ + 1] = { 0 };

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	if (lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)) == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() error");
		goto error_out;
	}

	if (extract_xpath_key_value(path_buffer, "interface", "name", interface_name_buffer, sizeof(interface_name_buffer))) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "extract_xpath_key_value() error");
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s; Interface Name: %s", path_buffer, interface_name_buffer);

	switch (change_ctx->operation) {
	case SR_OP_CREATED:
		break;
	case SR_OP_MODIFIED:
		break;
	case SR_OP_DELETED:
		break;
	case SR_OP_MOVED:
		break;
	}

	goto out;

error_out:
	error = -1;
out:
	return error;
}

int interfaces_subscription_change_interfaces_interface(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;

	char change_xpath_buffer[PATH_MAX] = { 0 };

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Aborting changes for %s", xpath);
		goto error_out;
	} else if (event == SR_EV_CHANGE || event == SR_EV_ENABLED) {
		// ipv4/dhcp-client
		if (snprintf(change_xpath_buffer, sizeof(change_xpath_buffer), "%s/subinterfaces/subinterface/ipv4/config/dhcp-client", xpath) < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "");
			goto error_out;
		}
		if (iterate_changes(private_data, session, change_xpath_buffer, interfaces_interface_ipv4_change_dhcp_client)) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "iterate_changes() error");
			goto error_out;
		}
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:
	return error;
}
