#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#

from plano import *

@command
def build():
    check_program("gcc", "I can't find gcc.  Run 'dnf install gcc'.")

    run("gcc client.c -o client -g -O2 -std=c99 -fno-omit-frame-pointer")
    run("gcc server.c -o server -g -O2 -std=c99 -fno-omit-frame-pointer -lunwind")

@command
def run_(gdb=False):
    build()

    if gdb:
        with start("gdb -ex run -ex '\''thread apply all bt'\'' -ex '\''set confirm off'\'' --args server"):
            sleep(2)
            run("client");
    else:
        with start("server"):
            sleep(1)
            run("client");

@command
def clean():
    remove("client")
    remove("server")
    remove("__pycache__")
