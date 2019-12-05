#! /bin/bash

# Copyright (c) 2019, Arm Ltd.  All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.




# This script comes with no warranty.

# To run, just say:
# ./flatten.sh

# This takes the history of origin/master and creates branch called "new" with
# the rewritten history. It takes about 20s on my laptop. The "new" branch has a
# flat git history (that is, a series of commits with only one parent).
# Flattening is done for merge commits by taking the content of the commit as it
# is at the merge commit.

# The commit message is preserved for mainline non-merge commits.

# For merge commits, a new commit message is made by combining the commit
# messages of the non-mainline commits.

# Authorship (name, email, date) is taken from the second parent of the merge.

# set -x
set -e -u

# First-order assumptions:
#
# * Pull requests are authored by one person.

# Known deficiencies:
#
# * The commit messages are ugly. These can be tidied up fairly easily once a
#   format is agreed.
#
# * Individual commits within PRs are lost. This is somewhat fundamental, and
#   could be worked around with some effort; it would require rebasing a number
#   of patches, which is risky and time consuming, so instead the working tree
#   is taken as-is from the merge commit.


START_COMMIT=origin/master

makeparent() {
    if [ ! -z "$PARENT" ];
    then
        echo -p $PARENT;
    fi
}

git log --format=%H --reverse --first-parent ${START_COMMIT} | while read SHA
do
    echo Commit: $SHA

    # (ew. But ok.)
    NPARENTS=$(git rev-list -n1 --parents $SHA | xargs -n1 | tail -n+2 | wc -l )

    AUTHOR_SHA=$SHA

    if [[ $NPARENTS = 1 || $NPARENTS = 0 ]]
    then
        git show --no-patch --format='%s%n%b' "$SHA" > message.txt
    elif [[ $NPARENTS = 2 ]]
    then
        # Take second parent of the merge and assume that this is the author.
        AUTHOR_SHA=${SHA}^2

        # Merge commit
        {
            git show --no-patch --format=%b "$SHA"
            echo
            echo "Original merge commit: $SHA"
            echo
            echo "######"
            echo 
            git log ${SHA}^1..${SHA}^2 \
                --format='%s%n%b%n%nOriginal commit: %H%n-------%n'
        } > message.txt
    else
        echo "Unexpected number of parents for $SHA: $NPARENTS"
        exit 2
    fi
    
    # Authorship is taken from the second parent of the merge (i.e. the person who wrote the code).
    # The committer is taken from the commiter of the merge (i.e. the person who merged on github).
    export GIT_AUTHOR_NAME="$(git show --no-patch --format=format:%an $AUTHOR_SHA)"
    export GIT_AUTHOR_EMAIL="$(git show --no-patch --pretty=format:%ae $AUTHOR_SHA)"
    export GIT_AUTHOR_DATE="$(git show --no-patch --pretty=format:%aD $AUTHOR_SHA)"
    export GIT_COMMITTER_NAME="$(git show --no-patch --pretty=format:%cn $SHA)"
    export GIT_COMMITTER_EMAIL="$(git show --no-patch --pretty=format:%ce $SHA)"
    export GIT_COMMITTER_DATE="$(git show --no-patch --pretty=format:%cd $SHA)"

    PARENT=$(
        git commit-tree $(makeparent) -F message.txt ${SHA}^{tree}
    )

    echo "New commit: $PARENT"
    # Note: we're in a subshell because of the pipe into the while loop.
    # Variables aren't preserved outside, so we'll just save this into a file.
    echo "$PARENT" > newcommit
done

git checkout -B new $(cat newcommit)
git reset --hard HEAD