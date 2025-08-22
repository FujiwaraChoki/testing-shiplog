#!/usr/bin/env node

const fs = require('fs');
const path = require('path');
const readline = require('readline');

// Colors for beautiful terminal output
const colors = {
  reset: '\x1b[0m',
  bright: '\x1b[1m',
  dim: '\x1b[2m',
  red: '\x1b[31m',
  green: '\x1b[32m',
  yellow: '\x1b[33m',
  blue: '\x1b[34m',
  magenta: '\x1b[35m',
  cyan: '\x1b[36m',
  white: '\x1b[37m',
  bgRed: '\x1b[41m',
  bgGreen: '\x1b[42m',
  bgYellow: '\x1b[43m',
  bgBlue: '\x1b[44m'
};

// Todo storage file
const TODO_FILE = "todos.json";

class TodoApp {
  constructor() {
    this.todos = this.loadTodos();
    this.rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout
    });
  }

  loadTodos() {
    try {
      if (fs.existsSync(TODO_FILE)) {
        const data = fs.readFileSync(TODO_FILE, 'utf8');
        return JSON.parse(data);
      }
    } catch (error) {
      console.error(`${colors.red}Error loading todos:${colors.reset}`, error.message);
    }
    return [];
  }

  saveTodos() {
    try {
      fs.writeFileSync(TODO_FILE, JSON.stringify(this.todos, null, 2));
    } catch (error) {
      console.error(`${colors.red}Error saving todos:${colors.reset}`, error.message);
    }
  }

  addTodo(description, priority = 'medium', dueDate = null) {
    const todo = {
      id: Date.now().toString(),
      description: description.trim(),
      completed: false,
      priority: priority,
      dueDate: dueDate,
      createdAt: new Date().toISOString(),
      completedAt: null
    };

    this.todos.push(todo);
    this.saveTodos();
    console.log(`${colors.green}✓${colors.reset} Todo added: ${colors.cyan}${description}${colors.reset}`);
  }

  listTodos(filter = 'all') {
    if (this.todos.length === 0) {
      console.log(`${colors.yellow}No todos found.${colors.reset}`);
      return;
    }

    let filteredTodos = this.todos;

    switch (filter) {
      case 'pending':
        filteredTodos = this.todos.filter(todo => !todo.completed);
        break;
      case 'completed':
        filteredTodos = this.todos.filter(todo => todo.completed);
        break;
      case 'high':
        filteredTodos = this.todos.filter(todo => todo.priority === 'high');
        break;
      case 'medium':
        filteredTodos = this.todos.filter(todo => todo.priority === 'medium');
        break;
      case 'low':
        filteredTodos = this.todos.filter(todo => todo.priority === 'low');
        break;
    }

    if (filteredTodos.length === 0) {
      console.log(`${colors.yellow}No todos found for filter: ${filter}${colors.reset}`);
      return;
    }

    console.log(`\n${colors.bright}${colors.blue}📋 Todo List${colors.reset} (${filteredTodos.length} items)\n`);

    filteredTodos.forEach((todo, index) => {
      const status = todo.completed ? `${colors.green}✓${colors.reset}` : `${colors.yellow}○${colors.reset}`;
      const priorityColor = this.getPriorityColor(todo.priority);
      const priority = `${priorityColor}${todo.priority.toUpperCase()}${colors.reset}`;
      const dueDate = todo.dueDate ? ` ${colors.magenta}📅 ${todo.dueDate}${colors.reset}` : '';
      const completedDate = todo.completedAt ? ` ${colors.green}✅ ${new Date(todo.completedAt).toLocaleDateString()}${colors.reset}` : '';

      console.log(`${status} ${index + 1}. ${todo.description} ${priority}${dueDate}${completedDate}`);
    });
    console.log('');
  }

  getPriorityColor(priority) {
    switch (priority) {
      case 'high': return colors.red;
      case 'medium': return colors.yellow;
      case 'low': return colors.green;
      default: return colors.white;
    }
  }

  completeTodo(id) {
    const todo = this.todos.find(t => t.id === id);
    if (todo) {
      if (todo.completed) {
        todo.completed = false;
        todo.completedAt = null;
        console.log(`${colors.yellow}○${colors.reset} Todo unmarked: ${colors.cyan}${todo.description}${colors.reset}`);
      } else {
        todo.completed = true;
        todo.completedAt = new Date().toISOString();
        console.log(`${colors.green}✓${colors.reset} Todo completed: ${colors.cyan}${todo.description}${colors.reset}`);
      }
      this.saveTodos();
    } else {
      console.log(`${colors.red}✗${colors.reset} Todo not found.`);
    }
  }

  deleteTodo(id) {
    const index = this.todos.findIndex(t => t.id === id);
    if (index !== -1) {
      const deletedTodo = this.todos.splice(index, 1)[0];
      this.saveTodos();
      console.log(`${colors.red}🗑️${colors.reset} Todo deleted: ${colors.cyan}${deletedTodo.description}${colors.reset}`);
    } else {
      console.log(`${colors.red}✗${colors.reset} Todo not found.`);
    }
  }

  searchTodos(query) {
    const results = this.todos.filter(todo =>
      todo.description.toLowerCase().includes(query.toLowerCase())
    );

    if (results.length === 0) {
      console.log(`${colors.yellow}No todos found matching: ${query}${colors.reset}`);
      return;
    }

    console.log(`\n${colors.bright}${colors.blue}🔍 Search Results${colors.reset} for "${query}" (${results.length} items)\n`);

    results.forEach((todo, index) => {
      const status = todo.completed ? `${colors.green}✓${colors.reset}` : `${colors.yellow}○${colors.reset}`;
      const priorityColor = this.getPriorityColor(todo.priority);
      const priority = `${priorityColor}${todo.priority.toUpperCase()}${colors.reset}`;

      console.log(`${status} ${index + 1}. ${todo.description} ${priority}`);
    });
    console.log('');
  }

  showStats() {
    const total = this.todos.length;
    const completed = this.todos.filter(t => t.completed).length;
    const pending = total - completed;
    const highPriority = this.todos.filter(t => t.priority === 'high' && !t.completed).length;

    console.log(`\n${colors.bright}${colors.blue}📊 Todo Statistics${colors.reset}\n`);
    console.log(`Total: ${colors.cyan}${total}${colors.reset}`);
    console.log(`Completed: ${colors.green}${completed}${colors.reset}`);
    console.log(`Pending: ${colors.yellow}${pending}${colors.reset}`);
    console.log(`High Priority Pending: ${colors.red}${highPriority}${colors.reset}`);

    if (total > 0) {
      const completionRate = ((completed / total) * 100).toFixed(1);
      console.log(`Completion Rate: ${colors.cyan}${completionRate}%${colors.reset}`);
    }
    console.log('');
  }

  showHelp() {
    console.log(`\n${colors.bright}${colors.blue}🚀 Todo CLI - Help${colors.reset}\n`);
    console.log(`${colors.cyan}Commands:${colors.reset}`);
    console.log(`  ${colors.green}add${colors.reset} <description> [priority] [due-date]  - Add a new todo`);
    console.log(`  ${colors.green}list${colors.reset} [filter]                          - List todos (all/pending/completed/high/medium/low)`);
    console.log(`  ${colors.green}complete${colors.reset} <id>                          - Toggle todo completion`);
    console.log(`  ${colors.green}delete${colors.reset} <id>                           - Delete a todo`);
    console.log(`  ${colors.green}search${colors.reset} <query>                        - Search todos`);
    console.log(`  ${colors.green}stats${colors.reset}                                 - Show statistics`);
    console.log(`  ${colors.green}help${colors.reset}                                  - Show this help`);
    console.log(`  ${colors.green}quit${colors.reset}                                  - Exit the app\n`);

    console.log(`${colors.cyan}Examples:${colors.reset}`);
    console.log(`  add "Buy groceries" high "2024-01-15"`);
    console.log(`  list pending`);
    console.log(`  complete 1234567890`);
    console.log(`  search "meeting"`);
    console.log(`  stats\n`);

    console.log(`${colors.cyan}Priorities:${colors.reset} high, medium, low (default: medium)`);
    console.log(`${colors.cyan}Date Format:${colors.reset} YYYY-MM-DD\n`);
  }

  async promptUser() {
    return new Promise((resolve) => {
      this.rl.question(`${colors.cyan}todo>${colors.reset} `, (input) => {
        resolve(input.trim());
      });
    });
  }

  async run() {
    console.log(`${colors.bright}${colors.blue}🚀 Welcome to Todo CLI!${colors.reset}`);
    console.log(`${colors.dim}Type 'help' for available commands${colors.reset}\n`);

    while (true) {
      try {
        const input = await this.promptUser();

        if (!input) continue;

        const parts = input.split(' ');
        const command = parts[0].toLowerCase();
        const args = parts.slice(1);

        switch (command) {
          case 'add':
            if (args.length === 0) {
              console.log(`${colors.red}Error:${colors.reset} Please provide a description for the todo.`);
              break;
            }
            const description = args[0];
            const priority = args[1] || 'medium';
            const dueDate = args[2] || null;
            this.addTodo(description, priority, dueDate);
            break;

          case 'list':
            const filter = args[0] || 'all';
            this.listTodos(filter);
            break;

          case 'complete':
            if (args.length === 0) {
              console.log(`${colors.red}Error:${colors.reset} Please provide a todo ID.`);
              break;
            }
            this.completeTodo(args[0]);
            break;

          case 'delete':
            if (args.length === 0) {
              console.log(`${colors.red}Error:${colors.reset} Please provide a todo ID.`);
              break;
            }
            this.deleteTodo(args[0]);
            break;

          case 'search':
            if (args.length === 0) {
              console.log(`${colors.red}Error:${colors.reset} Please provide a search query.`);
              break;
            }
            this.searchTodos(args.join(' '));
            break;

          case 'stats':
            this.showStats();
            break;

          case 'help':
            this.showHelp();
            break;

          case 'quit':
          case 'exit':
            console.log(`\n${colors.green}👋 Goodbye!${colors.reset}\n`);
            this.rl.close();
            return;

          default:
            console.log(`${colors.red}Unknown command:${colors.reset} ${command}`);
            console.log(`Type 'help' for available commands.`);
        }
      } catch (error) {
        console.error(`${colors.red}Error:${colors.reset}`, error.message);
      }
    }
  }
}

function main() {
  const app = new TodoApp();
  app.run().catch(error => {
    console.error(`${colors.red}Fatal error:${colors.reset}`, error);
    process.exit(1);
  });
}

// Handle process termination gracefully
process.on('SIGINT', () => {
  console.log(`\n${colors.green}👋 Goodbye!${colors.reset}\n`);
  process.exit(0);
});

if (require.main === module) {
  main();
}
