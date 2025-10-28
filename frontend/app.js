document.addEventListener('DOMContentLoaded', () => {

    const apiBase = 'http://localhost:8080/api';
    const input = document.getElementById('searchInput');
    const dropdown = document.getElementById('suggestionsDropdown');
    const list = document.getElementById('suggestionsList');
    const none = document.getElementById('noSuggestions');
    const clearBtn = document.getElementById('clearBtn');
    const statusIndicator = document.getElementById('apiStatus');
    const statusText = document.querySelector('.status-indicator__text');

    let selectedIndex = -1;
    let timer;
    let connectionRetries = 0;
    const maxRetries = 3;

    // --- API & DATA FUNCTIONS ---

    async function testConnection() {
        if (!statusIndicator || !statusText) return;
        try {
            const response = await fetch(`${apiBase}/health`);
            if (!response.ok) throw new Error(`HTTP ${response.status}`);
            
            statusText.textContent = 'Connected to API';
            statusIndicator.classList.remove('status-indicator--disconnected');
            statusIndicator.classList.add('status-indicator--connected');
            connectionRetries = 0;
        } catch (error) {
            statusText.textContent = `Connection failed`;
            statusIndicator.classList.remove('status-indicator--connected');
            statusIndicator.classList.add('status-indicator--disconnected');
            if (connectionRetries < maxRetries) {
                connectionRetries++;
                setTimeout(testConnection, Math.pow(2, connectionRetries) * 1000);
            }
        }
    }

    async function recordSearch(query) {
        if (!query.trim()) return;
        try {
            await fetch(`${apiBase}/search`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ query: query.trim() })
            });
        } catch (error) {
            console.error('Failed to record search:', error);
        }
    }

    // --- UI FUNCTIONS ---

    function displaySuggestions(suggestions) {
        list.innerHTML = '';
        selectedIndex = -1;

        if (!suggestions || suggestions.length === 0) {
            dropdown.style.display = 'none';
            return;
        }

        suggestions.forEach((suggestion, index) => {
            const div = document.createElement('div');
            div.textContent = suggestion;
            div.classList.add('suggestion-item');
            div.dataset.index = index;
            div.dataset.suggestion = suggestion; // Store suggestion text
            list.appendChild(div);
        });

        dropdown.style.display = 'block';
    }

    function updateSelectedItem() {
        const items = list.children;
        for (let i = 0; i < items.length; i++) {
            if (i === selectedIndex) {
                items[i].classList.add('suggestion-item--selected');
                items[i].scrollIntoView({ block: 'nearest' });
            } else {
                items[i].classList.remove('suggestion-item--selected');
            }
        }
    }

    function hideSuggestions() {
        dropdown.style.display = 'none';
        selectedIndex = -1;
    }

    function selectSuggestion(suggestionText) {
        console.log('Selecting suggestion:', suggestionText); // Debug log
        
        if (!suggestionText) return;
        
        // Set input value
        input.value = suggestionText;
        
        // Hide dropdown immediately
        hideSuggestions();
        
        // Record search
        recordSearch(suggestionText);
        
        // Focus input and move cursor to end
        input.focus();
        setTimeout(() => {
            try {
                input.setSelectionRange(input.value.length, input.value.length);
            } catch (e) {
                // Ignore errors
            }
        }, 0);
    }

    // --- EVENT HANDLERS ---

    // Input handling
    input.addEventListener('input', (e) => {
        clearTimeout(timer);
        const query = input.value.trim();

        if (!query) {
            hideSuggestions();
            return;
        }

        timer = setTimeout(async () => {
            try {
                const response = await fetch(`${apiBase}/suggest?prefix=${encodeURIComponent(query)}`);
                if (!response.ok) throw new Error(`HTTP ${response.status}`);
                const data = await response.json();
                displaySuggestions(data.suggestions || []);
            } catch (error) {
                console.error('Error fetching suggestions:', error);
                hideSuggestions();
            }
        }, 300);
    });

    // Keyboard navigation
    input.addEventListener('keydown', (e) => {
        const items = list.children;
        
        if (dropdown.style.display !== 'block' || items.length === 0) {
            if (e.key === 'Enter') {
                recordSearch(input.value);
            }
            return;
        }

        switch (e.key) {
            case 'PageDown':
            case 'ArrowDown':
                e.preventDefault();
                selectedIndex = (selectedIndex + 1) % items.length;
                updateSelectedItem();
                break;
                
            case 'ArrowUp':
                e.preventDefault();
                selectedIndex = (selectedIndex - 1 + items.length) % items.length;
                updateSelectedItem();
                break;
                
            case 'Enter':
                e.preventDefault();
                if (selectedIndex >= 0 && selectedIndex < items.length) {
                    const suggestion = items[selectedIndex].dataset.suggestion;
                    selectSuggestion(suggestion);
                } else {
                    hideSuggestions();
                    recordSearch(input.value);
                }
                break;
                
            case 'Escape':
                e.preventDefault();
                hideSuggestions();
                break;
        }
    });

    // Single click handler for suggestions
    list.addEventListener('click', (e) => {
        console.log('Click detected on list'); // Debug log
        
        const item = e.target.closest('.suggestion-item');
        if (item) {
            console.log('Clicked item:', item.textContent); // Debug log
            e.preventDefault();
            e.stopPropagation();
            
            const suggestion = item.dataset.suggestion || item.textContent;
            selectSuggestion(suggestion);
        }
    });

    // Prevent dropdown from closing when clicking on it
    dropdown.addEventListener('mousedown', (e) => {
        e.preventDefault();
    });

    // Close dropdown when clicking outside
    document.addEventListener('click', (e) => {
        if (!input.contains(e.target) && !dropdown.contains(e.target)) {
            hideSuggestions();
        }
    });

    // Clear button
    clearBtn.addEventListener('click', (e) => {
        e.preventDefault();
        input.value = '';
        hideSuggestions();
        input.focus();
    });

    // Focus handling
    input.addEventListener('focus', () => {
        if (input.value.trim() && list.children.length > 0) {
            dropdown.style.display = 'block';
        }
    });

    // Blur handling with delay
    let blurTimeout;
    input.addEventListener('blur', () => {
        blurTimeout = setTimeout(() => {
            hideSuggestions();
        }, 150);
    });

    // Cancel blur timeout if clicking on dropdown
    dropdown.addEventListener('mouseenter', () => {
        if (blurTimeout) {
            clearTimeout(blurTimeout);
        }
    });

    // --- INITIALIZATION ---
    testConnection();
    
    console.log('Autocomplete system initialized'); // Debug log
});