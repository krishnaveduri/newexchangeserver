/*
 * Description: 
 *     History: yang@haipo.me, 2017/04/04, create
 */

# ifndef _ME_PERSIST_H_
# define _ME_PERSIST_H_

# include <time.h>
# include "me_market.h"

int init_persist(void);

int init_from_db(void);
int dump_to_db(time_t timestamp);
int make_slice(time_t timestamp);
int clear_slice(time_t timestamp);
int push_cancelled_order_to_db(order_t *order);
int load_assets_from_db(void);
int load_markets_from_db(void);
int add_asset(const char *asset_name, uint64_t prec_save, uint64_t prec_show);
int delete_asset(const char *asset_name);
int add_market(const char *market_name, const char *stock_name, uint64_t stock_prec, const char *money_name, uint64_t money_prec, uint64_t fee_prec, const char *min_amount);
int delete_market(const char *market_name);

# endif

