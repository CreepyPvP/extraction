-- vim.opt.errorformat = " %#%f(%l\\,%c):\\ %m"
vim.opt.makeprg = "build.bat"
vim.keymap.set("n", "<", "<cmd>tabnew term://game.exe<CR>");
vim.keymap.set("n", ">", "<cmd>tabnew term://run_server.bat<CR>");

vim.keymap.set("n", "<A-c>", "<cmd>make<CR>");
vim.keymap.set("n", "<A-o>", "<cmd>!devenv game.exe<CR>");

