// The URL where our backend server is running
const API_URL = 'http://localhost:3000';

// Call loadStatus when the page first loads
document.addEventListener('DOMContentLoaded', () => {
    loadStatus();
});

async function parkVehicle() {
    let type = document.getElementById("vehicleType").value;
    let number = document.getElementById("vehicleNumber").value.toUpperCase();

    if (number === "") {
        alert("Please enter vehicle number!");
        return;
    }

    try {
        const response = await fetch(API_URL + '/park', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ type: type, number: number }),
        });

        const data = await response.json();
        alert(data.message); // Show success or error message from server

        if (response.ok) {
            loadStatus(); // Refresh the status list
            document.getElementById("vehicleNumber").value = ''; // Clear input
        }
    } catch (error) {
        console.error('Error parking vehicle:', error);
        alert('Could not connect to the server.');
    }
}

async function exitVehicle() {
    let number = document.getElementById("exitNumber").value.toUpperCase();

    if (number === "") {
        alert("Please enter vehicle number!");
        return;
    }

    try {
        const response = await fetch(API_URL + '/exit', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ number: number }),
        });

        const data = await response.json();
        alert(data.message); // Show success or error message from server

        if (response.ok) {
            loadStatus(); // Refresh the status list
            document.getElementById("exitNumber").value = ''; // Clear input
        }
    } catch (error) {
        console.error('Error exiting vehicle:', error);
        alert('Could not connect to the server.');
    }
}

async function loadStatus() {
    const statusList = document.getElementById("statusList");
    statusList.innerHTML = "<li>Loading...</li>"; // Show loading feedback

    try {
        const response = await fetch(API_URL + '/status');
        const data = await response.json();

        statusList.innerHTML = ""; // Clear the list

        if (data.length === 0) {
            statusList.innerHTML = "<li>Parking lot status is unavailable.</li>";
            return;
        }

        // Build the list from the server data
        data.forEach(spot => {
            const li = document.createElement('li');
            if (spot.status === 'FREE') {
                li.textContent = `Spot ${spot.spot}: FREE`;
                li.style.color = '#a5d6a7'; // Lighter green for free
            } else {
                li.textContent = `Spot ${spot.spot}: OCCUPIED by ${spot.vehicleType} (${spot.vehicleNumber})`;
                li.style.color = '#ef9a9a'; // Lighter red for occupied
            }
            statusList.appendChild(li);
        });

    } catch (error) {
        console.error('Error loading status:', error);
        statusList.innerHTML = "<li>Could not load parking status. Is the server running?</li>";
    }
}