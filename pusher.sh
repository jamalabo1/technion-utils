#!/usr/bin/env bash
set -euo pipefail

# ←—— CONFIGURE HERE —————————————————————————————————————
REMOTE_NAME="utils"                        # name of the git remote
LOCAL_BRANCH="utils-latex"                 # name of the local branch to maintain
REMOTE_BRANCH="latex"                # name of the branch on the remote
PATHS=(.vscode helpers fonts .gitignore pusher.sh)   # files/folders to include
# ——————————————————————————————————————————————————————→

# 1. Stash any local changes (tracked + untracked)
stash_ref=""
if ! git diff-index --quiet HEAD --; then
  stash_ref=$(git stash push -u -m "pre-${LOCAL_BRANCH}-stash")
fi

# 2. Remember current branch
ORIGINAL_BRANCH=$(git rev-parse --abbrev-ref HEAD)

cleanup() {
  git checkout "$ORIGINAL_BRANCH" > /dev/null 2>&1
  if [ -n "$stash_ref" ]; then
    git stash pop > /dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

# 3. Switch to (or create) the local target branch
JUST_CREATED_BRANCH=0
if git show-ref --quiet refs/heads/"$LOCAL_BRANCH"; then
  git checkout "$LOCAL_BRANCH" > /dev/null 2>&1
else
  git checkout --orphan "$LOCAL_BRANCH" > /dev/null 2>&1
  git rm -rf . > /dev/null 2>&1
  JUST_CREATED_BRANCH=1
fi

# 4. Pull in only the specified paths from the original branch
git checkout "$ORIGINAL_BRANCH" -- "${PATHS[@]}" > /dev/null 2>&1

# 5. Commit & push if there are changes
if ! git diff-index --quiet HEAD --; then
  git add "${PATHS[@]}" > /dev/null 2>&1
  git commit --quiet -m "Automated $LOCAL_BRANCH commit $REMOTE_BRANCH @ $(date +%Y-%m-%d)"
  if [ "$JUST_CREATED_BRANCH" -eq 1 ]; then
    git push --quiet --force "$REMOTE_NAME" "$LOCAL_BRANCH":"$REMOTE_BRANCH" > /dev/null 2>&1
  else
    git push --quiet "$REMOTE_NAME" "$LOCAL_BRANCH":"$REMOTE_BRANCH" > /dev/null 2>&1
  fi
else
  echo "No changes in ${PATHS[*]}; nothing to commit."
fi

# 6. Restore original state
trap - EXIT
cleanup

echo "✅ '$LOCAL_BRANCH' pushed to '$REMOTE_NAME/$REMOTE_BRANCH' and workspace restored."