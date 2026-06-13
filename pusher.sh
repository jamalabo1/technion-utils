#!/usr/bin/env bash
set -euo pipefail

# ←—— CONFIGURE HERE —————————————————————————————————————
REMOTE_NAME="utils"                        # name of the git remote
LOCAL_BRANCH="utils-latex"                 # name of the local branch to maintain
REMOTE_BRANCH="latex"                # name of the branch on the remote
PATHS=(.vscode helpers fonts .gitignore pusher.sh)   # files/folders to include
# ——————————————————————————————————————————————————————→

# 1. Build the public branch in a temporary worktree so the notes tree never
#    gets replaced by the small utils branch.
SOURCE_REF=$(git rev-parse --verify HEAD)
WORKTREE=$(mktemp -d "${TMPDIR:-/tmp}/${LOCAL_BRANCH}.XXXXXX")

cleanup() {
  git worktree remove --force "$WORKTREE" > /dev/null 2>&1 || true
}
trap cleanup EXIT

# 2. Check out (or create) the local target branch in the temporary worktree.
JUST_CREATED_BRANCH=0
if git show-ref --quiet refs/heads/"$LOCAL_BRANCH"; then
  git worktree add --quiet "$WORKTREE" "$LOCAL_BRANCH" > /dev/null 2>&1
else
  git worktree add --detach --quiet "$WORKTREE" "$SOURCE_REF" > /dev/null 2>&1
  git -C "$WORKTREE" checkout --orphan "$LOCAL_BRANCH" > /dev/null 2>&1
  git -C "$WORKTREE" rm -rf . > /dev/null 2>&1 || true
  JUST_CREATED_BRANCH=1
fi

# 3. Replace the target branch contents with only the specified paths.
git -C "$WORKTREE" rm -rf --ignore-unmatch . > /dev/null 2>&1
git -C "$WORKTREE" checkout "$SOURCE_REF" -- "${PATHS[@]}" > /dev/null 2>&1
git -C "$WORKTREE" add -A > /dev/null 2>&1

# 4. Commit & push if there are changes.
if ! git -C "$WORKTREE" diff --cached --quiet; then
  git -C "$WORKTREE" commit --quiet -m "Automated $LOCAL_BRANCH commit $REMOTE_BRANCH @ $(date +%Y-%m-%d)"
  if [ "$JUST_CREATED_BRANCH" -eq 1 ]; then
    git -C "$WORKTREE" push --quiet --force "$REMOTE_NAME" "$LOCAL_BRANCH":"$REMOTE_BRANCH" > /dev/null 2>&1
  else
    git -C "$WORKTREE" push --quiet "$REMOTE_NAME" "$LOCAL_BRANCH":"$REMOTE_BRANCH" > /dev/null 2>&1
  fi
else
  echo "No changes in ${PATHS[*]}; nothing to commit."
fi

# 5. Remove the temporary worktree.
trap - EXIT
cleanup

echo "✅ '$LOCAL_BRANCH' pushed to '$REMOTE_NAME/$REMOTE_BRANCH' and workspace restored."
