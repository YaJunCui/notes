# 删除github上文件夹

 解决已经加入 ignore 的文件夹无法从远程仓库删除的问题


1 如果此文件夹已被加入 git 追踪，那么删除方法很简单，只需要将此文件夹删掉，然后提交一下就可以了。

2 如果次文件夹曾经被加入过 git 追踪，现在被加入 .gitignore 里了，但是 github 上还有此文件夹。对于这种情况，稍微有点复杂，因为已经加入.gitignore的文件或文件夹，无法对其进行提交了，哪怕是将其删除，都无法提交。我们用以下方法可以很好的解决这个问题：

```git
git rm -r --cached some-directory
git commit -m 'Remove the now ignored directory "some-directory"'
git push origin master
```

[参考: Remove directory from remote repository after adding them to .gitignore](http://stackoverflow.com/questions/7927230/remove-directory-from-remote-repository-after-adding-them-to-gitignore)