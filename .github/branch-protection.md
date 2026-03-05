# Branch Protection Setup

After pushing, run this once to lock `main` so all changes must go through a reviewed PR:

```bash
gh api repos/YOUR_ORG/WaveForge/branches/main/protection \
  --method PUT \
  --field required_status_checks=null \
  --field enforce_admins=false \
  --field "required_pull_request_reviews[required_approving_review_count]=1" \
  --field "required_pull_request_reviews[dismiss_stale_reviews]=true" \
  --field "restrictions=null"
```

Replace `YOUR_ORG` with the actual org name.

Or set it in GitHub UI:
Settings → Branches → Add branch protection rule → Branch name: `main`
- [x] Require a pull request before merging
- [x] Require approvals: 1
- [x] Dismiss stale pull request approvals when new commits are pushed
- [x] Do not allow bypassing the above settings
