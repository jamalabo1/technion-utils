#!/bin/bash
set -e

# Configuration
PUBLIC_REMOTE="utils"
PRIVATE_REMOTE="origin"
PRIVATE_BRANCH=$(git branch --show-current)
TMP_BRANCH="tmp-public-push"
PUBLIC_BRANCH="latex"

# Directories to include in the public push
INCLUDE_DIRS=(".vscode" "helpers" "fonts" ".gitignore" "pusher.sh")

# Ensure clean working state
if ! git diff --quiet || ! git diff --cached --quiet; then
  echo "⚠️ Please commit or stash your changes first."
  exit 1
fi

# Remove previous temp branch if exists
if git rev-parse --verify $TMP_BRANCH >/dev/null 2>&1; then
  git branch -D $TMP_BRANCH
fi

# Create a subtree with only the desired folders
echo "📦 Splitting tree..."

# Add each additional folder into the new branch
for dir in "${INCLUDE_DIRS[@]:1}"; do
git subtree split --prefix="${dir}" -b $TMP_BRANCH
  # git read-tree --prefix="$dir/" -u $TMP_BRANCH $(git subtree split --prefix="$dir")
done

  git commit -m "Add subtrees to $TMP_BRANCH"

# Push the temp branch to public remote
echo "🚀 Pushing public content to '$PUBLIC_REMOTE'..."
git push -f $PUBLIC_REMOTE $TMP_BRANCH:$PUBLIC_BRANCH

# Push full private branch to origin
echo "🔐 Pushing private branch '$PRIVATE_BRANCH' to '$PRIVATE_REMOTE'..."
git push $PRIVATE_REMOTE $PRIVATE_BRANCH

# Cleanup
git branch -D $TMP_BRANCH

echo "✅ Done!"