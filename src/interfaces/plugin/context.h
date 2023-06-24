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
#ifndef INTERFACES_PLUGIN_CONTEXT_H
#define INTERFACES_PLUGIN_CONTEXT_H

typedef struct module_change_s module_change_t;
typedef struct change_ctx_s change_ctx_t;
typedef struct interfaces_nl_ctx_s interfaces_nl_ctx_t;
typedef struct interfaces_ctx_s interfaces_ctx_t;
typedef struct interfaces_state_changes_ctx_s interfaces_state_changes_ctx_t;
typedef struct interfaces_mod_changes_ctx_s interfaces_mod_changes_ctx_t;
typedef struct interfaces_oper_ctx_s interfaces_oper_ctx_t;
typedef struct interfaces_startup_ctx_s interfaces_startup_ctx_t;

struct module_change_s {
    const char *path;       ///< Path to which the callback will be applied.
    sr_module_change_cb cb; ///< Module change callback.
};

struct change_ctx_s {
    const struct lyd_node *node; ///< Current changed libyang node.
    const char *previous_value;  ///< Previous node value.
    const char *previous_list;   ///< Previous list keys predicate.
    int previous_default;        ///< Previous value default flag.
    sr_change_oper_t operation;  ///< Operation being applied on the node.
};

struct interfaces_nl_ctx_s {
	int tmp;
};

struct interfaces_mod_changes_ctx_s {
	// libnl links data
	interfaces_nl_ctx_t nl_ctx;

	// temporary module changing data
	struct {
		struct {
			struct {
				uint8_t prefix_length;
				uint8_t prefix_set; ///< prefix_length has been set
			} address;
			struct {
				char *	link_layer_address;
				uint8_t link_layer_set; ///< link_layer_address has been set
			} neighbor;
		} ipv4;
		struct {
			struct {
				uint8_t prefix_length;
				uint8_t prefix_set; ///< prefix_length has been set
			} address;
			struct {
				char *	link_layer_address;
				uint8_t link_layer_set; ///< link_layer_address has been set
			} neighbor;
		} ipv6;
	} mod_data;
};

struct interfaces_state_changes_ctx_s {
	// libnl data
	interfaces_nl_ctx_t				nl_ctx;
};

struct interfaces_oper_ctx_s {
	// operational libnl context - refill cache of links
	interfaces_nl_ctx_t		nl_ctx;

	// state changes monitoring
	interfaces_state_changes_ctx_t	state_changes_ctx;
};

struct interfaces_startup_ctx_s {
	// startup DS
	sr_session_ctx_t *	startup_session;

	// libnl context
	interfaces_nl_ctx_t	nl_ctx;
};

struct interfaces_ctx_s {
	// startup data
	interfaces_startup_ctx_t	startup_ctx;

	// module changes data
	interfaces_mod_changes_ctx_t	mod_ctx;

	// operational data
	interfaces_oper_ctx_t		oper_ctx;
};

#endif // INTERFACES_PLUGIN_CONTEXT_H
