#!/bin/bash
/etc/init.d/postgresql restart
pm2 resurrect
