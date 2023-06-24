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
#include "plugin.h"

// subscription
#include "plugin/subscription/change.h"

_Atomic volatile int exit_application = 0;

int sr_plugin_init_cb(sr_session_ctx_t *running_session, void **private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = NULL;
	sr_conn_ctx_t *connection = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	// plugin
	interfaces_ctx_t *ctx = NULL;

	// init context
	ctx = malloc(sizeof(*ctx));
	if (!ctx) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "malloc error");
		goto error_out;
	}
	*ctx = (interfaces_ctx_t) { 0 };

	*private_data = ctx;

	// module changes
	module_change_t module_changes[] = {
		{
			OC_INTERFACES_INTERFACE_YANG_PATH,
			interfaces_subscription_change_interfaces_interface,
		},
	};

	// get connection
	connection = sr_session_get_connection(running_session);
	if (!connection) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_get_connection() error");
		goto error_out;
	}

	// start a session
	if(sr_session_start(connection, SR_DS_STARTUP, &startup_session)) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start() error");
		goto error_out;
	}

	ctx->startup_ctx.startup_session = startup_session;

	for (size_t i = 0; i < ARRAY_SIZE(module_changes); i++) {
		const module_change_t *change = &module_changes[i];

		SRPLG_LOG_INF(PLUGIN_NAME, "Subscribing module change callback %s", change->path);

		if (change->cb) {
			error = sr_module_change_subscribe(running_session, OC_INTERFACES_YANG_MODULE, change->path, change->cb, *private_data, 0, SR_SUBSCR_ENABLED, &subscription);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe() error for \"%s\" (%d): %s", change->path, error, sr_strerror(error));
				goto error_out;
			}
		}
	}

	goto out;

error_out:
	error = -1;
	SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while initializing the plugin (%d)", error);

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *running_session, void *private_data)
{
	interfaces_ctx_t *ctx = (interfaces_ctx_t *)private_data;
	exit_application = 1;
	free(ctx);
}
