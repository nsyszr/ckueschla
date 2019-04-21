/* 
 * Copyright (c) 2018 nsyszr.io. All rights reserved.
 */

#ifndef BUV_TYPES_H_
#define BUV_TYPES_H_

struct buv_brand
{
    int buvb_id;
    char buvb_name[255];
};

struct buv_vote
{
    int buvv_id;
    char buvv_name[255];
};

struct buv_user_vote
{
    int buvu_id;
    int buvu_vote_id;
    char buvu_user[255];
    int buvu_amount;
};

#endif /* BUV_TYPES_H_ */
