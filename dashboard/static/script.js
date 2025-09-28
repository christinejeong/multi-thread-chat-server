class DashboardUpdater {
    constructor() {
        this.statsEndpoint = '/api/stats';
        this.updateInterval = 2000;
        this.init();
    }
    
    init() {
        this.removeAllTooltips();
        this.updateStats();
        setInterval(() => this.updateStats(), this.updateInterval);
    }
    
    removeAllTooltips() {
        // Remove title attributes from all elements to prevent tooltips
        document.querySelectorAll('*').forEach(element => {
            element.removeAttribute('title');
        });
    }
    
    async updateStats() {
        try {
            const response = await fetch(this.statsEndpoint);
            if (!response.ok) throw new Error('Network response was not ok');
            
            const stats = await response.json();
            this.updateDashboard(stats);
            
        } catch (error) {
            console.error('Failed to fetch stats:', error);
            this.handleError();
        }
    }
    
    updateDashboard(stats) {
        // Update server status
        const statusElement = document.getElementById('server-status');
        statusElement.textContent = stats.server_status;
        statusElement.className = this.getStatusClass(stats.server_status);
        
        // Update numeric stats
        document.getElementById('active-clients').textContent = stats.active_clients;
        document.getElementById('total-rooms').textContent = stats.total_rooms;
        document.getElementById('messages-per-minute').textContent = stats.messages_per_minute;
        document.getElementById('uptime').textContent = stats.uptime;
        
        // Update client list
        this.updateList('client-list', stats.connected_clients);
        
        // Update room list
        this.updateList('room-list', stats.room_list);
        
        this.pulseUpdate();
    }
    
    updateList(listId, items) {
        const listElement = document.getElementById(listId);
        listElement.innerHTML = '';
        
        if (items && items.length > 0) {
            items.forEach(item => {
                const li = document.createElement('li');
                li.textContent = item;
                // Ensure no tooltip attributes
                li.removeAttribute('title');
                li.setAttribute('data-no-tooltip', 'true');
                listElement.appendChild(li);
            });
        } else {
            const li = document.createElement('li');
            li.textContent = listId === 'client-list' ? 'No clients connected' : 'No active rooms';
            li.style.color = '#666';
            li.style.fontStyle = 'italic';
            li.style.background = '#f8f8f8';
            li.style.borderLeft = '3px solid #ddd';
            li.removeAttribute('title');
            li.setAttribute('data-no-tooltip', 'true');
            listElement.appendChild(li);
        }
    }
    
    getStatusClass(status) {
        if (status.includes('Online')) return 'status-online';
        if (status.includes('Offline')) return 'status-offline';
        if (status.includes('Error')) return 'status-error';
        return 'status-offline';
    }
    
    handleError() {
        document.getElementById('server-status').textContent = 'Connection Failed';
        document.getElementById('server-status').className = 'status-error';
    }
    
    pulseUpdate() {
        const container = document.querySelector('.container');
        container.classList.remove('pulse-update');
        void container.offsetWidth;
        container.classList.add('pulse-update');
        
        setTimeout(() => {
            container.classList.remove('pulse-update');
        }, 500);
    }
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    // Remove tooltips from all elements immediately
    document.querySelectorAll('*').forEach(el => {
        el.removeAttribute('title');
    });
    
    new DashboardUpdater();
});